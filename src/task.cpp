#include "task.hpp"

#include <cassert>

Task::Task(FingerTable& fingerTable, ConnectionManager& connectionManager) : fingerTable(fingerTable), connectionManager(connectionManager) {
}

TaskState Task::getState() const {
    return state;
}

FindTask::FindTask(const Hash& target, FingerTable& fingerTable, ConnectionManager& connectionManager) : Task(fingerTable, connectionManager), target(target) {}

void FindTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    nextHop = fingerTable.getClosestPrecedingFinger(target);
    connectionManager.existsElseConnect(nextHop.value());
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
                                                           joinAddress(joinAddress),
                                                           findTask(Hash(ownAddress), fingerTable, connectionManager) {}

void JoinTask::update() {
    if (state == TaskState::FINISHED) {
        return;
    }

    if (connectionManager.isConnectionEstablished(joinAddress)) {
        findTask.init();
    }
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

    fingerTable.updateWithAddress(targetAddressOptional.value());
    fingerTable.setSuccessor(targetAddressOptional.value());
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
    connectionManager.existsElseConnect(fingerTable.getSuccessor());
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
