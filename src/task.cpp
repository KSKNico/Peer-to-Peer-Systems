#include "task.hpp"

#include <cassert>

Task::Task(FingerTable& fingerTable, ConnectionManager& connectionManager) : fingerTable(fingerTable), connectionManager(connectionManager) {
}

TaskState Task::getState() const {
    return state;
}

FindTask::FindTask(const Hash& target, FingerTable& fingerTable, ConnectionManager& connectionManager,
                   std::optional<Poco::Net::SocketAddress> nextHop = std::nullopt) : Task(fingerTable, connectionManager), target(target), nextHop(nextHop) {}

void FindTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    // the nextHop can be set when the search is to be started from a single known peer
    // i.e. when joining the network
    if (!nextHop.has_value()) {
        nextHop = fingerTable.getClosestPrecedingFinger(target);
        // connectionManager.existsElseConnect(nextHop.value());
    }

    state = TaskState::RUNNING;
}

void FindTask::update() {
    if (state == TaskState::FINISHED) {
        return;
    }

    if (!nextHop.has_value()) {
        return;
    }

    connectionManager.existsElseConnect(nextHop.value());
    if (connectionManager.isConnectionEstablished(nextHop.value())) {
        connectionManager.sendMessage(nextHop.value(), FindMessage(target));
        lastSentTo = nextHop.value();
        nextHop = std::nullopt;
    }
}

void FindTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    if (state == TaskState::FINISHED || from != lastSentTo) {
        return;
    }

    if (message->getType() != MessageType::FINDR) {
        return;
    }

    auto findMessage = dynamic_cast<const FindResponseMessage*>(message.get());

    if (target != findMessage->target) {
        return;
    }

    assert(!nextHop.has_value());

    auto referenceAddressHash = Hash(findMessage->referenceAddress);
    if (target.isBetween(Hash(from), referenceAddressHash)) {
        connectionManager.existsElseConnect(findMessage->referenceAddress);
        targetAddress = findMessage->referenceAddress;
        state = TaskState::FINISHED;
    } else {
        nextHop = findMessage->referenceAddress;
    }
}

std::optional<Poco::Net::SocketAddress> FindTask::getTargetAddress() const {
    return targetAddress;
}

JoinTask::JoinTask(const Poco::Net::SocketAddress& ownAddress,
                   const Poco::Net::SocketAddress& joinAddress,
                   FingerTable& fingerTable,
                   ConnectionManager& connectionManager) : Task(fingerTable, connectionManager),
                                                           ownAddress(ownAddress),
                                                           joinAddress(joinAddress),
                                                           findTask(Hash(ownAddress), fingerTable, connectionManager, joinAddress) {}

void JoinTask::update() {
    if (state == TaskState::FINISHED) {
        return;
    }

    if (connectionManager.isConnectionEstablished(joinAddress)) {
        findTask.init();
    }

    findTask.update();
}

void JoinTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    connectionManager.existsElseConnect(joinAddress);
    state = TaskState::RUNNING;
}

void JoinTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    findTask.processMessage(from, message);

    auto targetAddressOptional = findTask.getTargetAddress();
    if (!targetAddressOptional.has_value()) {
        return;
    }

    assert(findTask.getState() == TaskState::FINISHED);

    fingerTable.updateWithAddress(targetAddressOptional.value());
    fingerTable.setSuccessor(targetAddressOptional.value());

    // inform the new successor about the new predecessor (this peer)
    connectionManager.sendMessage(fingerTable.getSuccessor(), SetPredecessorMessage(ownAddress));
}

StabilizeTask::StabilizeTask(const Poco::Net::SocketAddress& ownAddress,
                             FingerTable& fingerTable, ConnectionManager& connectionManager) : Task(fingerTable, connectionManager), ownAddress(ownAddress) {}

void StabilizeTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    if (state == TaskState::FINISHED) {
        return;
    }

    if (message->getType() != MessageType::GPRER) {
        return;
    }

    assert(from == currentSuccessor);

    auto getPredecessorResponseMessage = dynamic_cast<const GetPredecessorResponseMessage*>(message.get());
    auto predecessor = getPredecessorResponseMessage->getPredecessor();

    if (predecessor != ownAddress) {
        assert(Hash(predecessor).isBetween(fingerTable.getOwnHash(), Hash(currentSuccessor)));
        predecessorOfSuccessor = predecessor;
        fingerTable.setSuccessor(predecessor);
    }
}

void StabilizeTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    // connectionManager.existsElseConnect(fingerTable.getSuccessor());
    currentSuccessor = fingerTable.getSuccessor();
    state = TaskState::RUNNING;
}

void StabilizeTask::update() {
    if (state == TaskState::FINISHED) {
        return;
    }

    // this if statements is only executed once the predecessor of the successor is known
    if (predecessorOfSuccessor.has_value()) {
        connectionManager.existsElseConnect(predecessorOfSuccessor.value());
        if (!connectionManager.isConnectionEstablished(predecessorOfSuccessor.value())) {
            return;
        }
        connectionManager.sendMessage(predecessorOfSuccessor.value(), SetPredecessorMessage(ownAddress));
        state = TaskState::FINISHED;
        return;
    }

    if (messageSent) {
        return;
    }

    connectionManager.existsElseConnect(currentSuccessor);

    if (!connectionManager.isConnectionEstablished(currentSuccessor)) {
        return;
    }

    assert(!messageSent);
    connectionManager.sendMessage(currentSuccessor, GetPredecessorMessage());
    messageSent = true;
}

FixFingersTask::FixFingersTask(const Poco::Net::SocketAddress& ownAddress, FingerTable& fingerTable, ConnectionManager& connectionManager) : Task(fingerTable, connectionManager), ownAddress(ownAddress) {
    findTasks.reserve(FingerTable::FINGER_TABLE_SIZE);
    for (std::size_t i = 0; i < FingerTable::FINGER_TABLE_SIZE; ++i) {
        findTasks.push_back(FindTask(fingerTable.getFingerHashValue(i), fingerTable, connectionManager));
    }

    assert(findTasks.size() == FingerTable::FINGER_TABLE_SIZE);
}

void FixFingersTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    for (auto& task : findTasks) {
        task.init();
    }

    state = TaskState::RUNNING;
}

void FixFingersTask::update() {
    if (state == TaskState::FINISHED) {
        return;
    }

    for (auto& task : findTasks) {
        task.update();
    }

    if (!std::all_of(findTasks.begin(),
                     findTasks.end(),
                     [](const FindTask& task) {
                         return task.getState() == TaskState::FINISHED;
                     })) {
        return;
    }

    for (std::size_t i = 0; i < FingerTable::FINGER_TABLE_SIZE; ++i) {
        auto address = findTasks[i].getTargetAddress();
        assert(address.has_value());
        fingerTable.updateWithAddress(address.value());
    }

    state = TaskState::FINISHED;
}

void FixFingersTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    for (auto& task : findTasks) {
        task.processMessage(from, message);
    }
}

CheckPredecessorTask::CheckPredecessorTask(const Poco::Net::SocketAddress& ownAddress, FingerTable& fingerTable, ConnectionManager& connectionManager) : Task(fingerTable, connectionManager), ownAddress(ownAddress) {}

void CheckPredecessorTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    state = TaskState::RUNNING;
    auto predecessor = fingerTable.getPredecessor();
    if (predecessor == ownAddress) {
        state = TaskState::FINISHED;
        return;
    }

    auto isEstablished = connectionManager.isConnectionEstablished(predecessor);

    if (!isEstablished) {
        fingerTable.setPredecessor(ownAddress);
        state = TaskState::FINISHED;
        return;
    }

    state = TaskState::FINISHED;
}

void CheckPredecessorTask::update() {}

void CheckPredecessorTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {}