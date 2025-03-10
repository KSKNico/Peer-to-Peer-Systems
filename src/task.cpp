#include "task.hpp"

#include <spdlog/spdlog.h>

#include <cassert>

Task::Task(FingerTable& fingerTable, ConnectionManager& connectionManager, const Poco::Net::SocketAddress& ownAddress) : 
fingerTable(fingerTable), connectionManager(connectionManager), ownAddress(ownAddress) {}

TaskState Task::getState() const {
    return state;
}

FindTask::FindTask(const Hash& target, 
                FingerTable& fingerTable, 
                ConnectionManager& connectionManager, 
                const Poco::Net::SocketAddress& ownAddress,
                std::optional<Poco::Net::SocketAddress> nextHop = std::nullopt) : 
                Task(fingerTable, connectionManager, ownAddress), target(target), nextHop(nextHop) {}

void FindTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    spdlog::get(ownAddress.toString())->debug("Finding target {}", target.toString());

    // the nextHop can be set when the search is to be started from a single known peer
    // i.e. when joining the network
    if (!nextHop.has_value()) {
        nextHop = fingerTable.getClosestPrecedingFinger(target);
        // connectionManager.existsElseConnect(nextHop.value());
    }

    state = TaskState::RUNNING;
}

void FindTask::update() {
    assert(messagesSent <= 10);

    if (state == TaskState::FINISHED) {
        return;
    }

    if (!nextHop.has_value()) {
        return;
    }

    if (nextHop.value() == ownAddress) {
        spdlog::get(ownAddress.toString())->debug("Target is for this peer");
        targetAddress = ownAddress;
        state = TaskState::FINISHED;
        nextHop = std::nullopt;
        return;
    }
    
    connectionManager.existsElseConnect(nextHop.value());
    if (connectionManager.isConnectionEstablished(nextHop.value())) {
        spdlog::get(ownAddress.toString())->debug("Looking for target {} at {}", target.toString(), nextHop.value().toString());
        connectionManager.sendMessage(nextHop.value(), FindMessage(target));
        this->messagesSent++;
        lastSentTo = nextHop.value();
        nextHop = std::nullopt;
    }
}

bool FindTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    if (state == TaskState::FINISHED || from != lastSentTo) {
        return false;
    }

    if (message->getType() != MessageType::FINDR) {
        return false;
    }

    auto findMessage = dynamic_cast<const FindResponseMessage*>(message.get());
    // std::cout << from.toString() << " ---> " << this->ownAddress.toString() << std::endl;
    spdlog::get(ownAddress.toString())->debug("Received find response message");

    if (target != findMessage->target) {
        spdlog::get(ownAddress.toString())->debug("Received find response message with wrong target");
        return false;
    }

    assert(!nextHop.has_value());

    auto referenceAddressHash = Hash(findMessage->referenceAddress);
    if (Hash(from) == referenceAddressHash) {
        connectionManager.existsElseConnect(findMessage->referenceAddress);
        targetAddress = findMessage->referenceAddress;

        spdlog::get(ownAddress.toString())->debug("Found target {} at {}", 
        target.toString(), 
        targetAddress.value().toString());
        
        state = TaskState::FINISHED;
    } else {
        nextHop = findMessage->referenceAddress;
    }
    return true;
}

std::optional<Poco::Net::SocketAddress> FindTask::getTargetAddress() const {
    return targetAddress;
}

JoinTask::JoinTask(const Poco::Net::SocketAddress& joinAddress,
                   FingerTable& fingerTable,
                   ConnectionManager& connectionManager,
                   const Poco::Net::SocketAddress& ownAddress) : Task(fingerTable, connectionManager, ownAddress),
                                                           joinAddress(joinAddress),
                                                           findTask(Hash(ownAddress), fingerTable, connectionManager, ownAddress, joinAddress) {}

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

    spdlog::get(ownAddress.toString())->info("Joining network using {}", joinAddress.toString());

    connectionManager.existsElseConnect(joinAddress);
    state = TaskState::RUNNING;
}

bool JoinTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    if (state == TaskState::FINISHED) {
        return false;
    }

    // the find task is responsible for processing the message
    bool findStatus = findTask.processMessage(from, message);

    auto targetAddressOptional = findTask.getTargetAddress();
    if (!targetAddressOptional.has_value()) {
        return findStatus;
    }

    assert(findTask.getState() == TaskState::FINISHED);

    if (targetAddressOptional.value() == ownAddress) {
        spdlog::get(ownAddress.toString())
        ->info("Joining network has failed because the target address is the same as the own address ({})", ownAddress.toString());

        state = TaskState::FINISHED;
        return true;
    }

    fingerTable.updateWithAddress(targetAddressOptional.value());
    fingerTable.setSuccessor(targetAddressOptional.value());

    // inform the new successor about the new predecessor (this peer)
    connectionManager.sendMessage(fingerTable.getSuccessor(), SetPredecessorMessage(ownAddress));
    this->messagesSent++;

    state = TaskState::FINISHED;
    spdlog::get(ownAddress.toString())->info("Joined network with successor {}", targetAddressOptional.value().toString());
    assert(fingerTable.getSuccessor() != ownAddress);
    return true;
}

StabilizeTask::StabilizeTask(FingerTable& fingerTable, 
ConnectionManager& connectionManager, 
const Poco::Net::SocketAddress& ownAddress) : Task(fingerTable, connectionManager, ownAddress) {}

bool StabilizeTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    if (state == TaskState::FINISHED) {
        return false;
    }

    if (message->getType() != MessageType::GPRER) {
        return false;
    }

    assert(from == currentSuccessor);

    auto getPredecessorResponseMessage = dynamic_cast<const GetPredecessorResponseMessage*>(message.get());
    auto predecessor = getPredecessorResponseMessage->getPredecessor();

    if (predecessor != ownAddress) {
        assert(Hash(predecessor).isBetween(fingerTable.getOwnHash(), Hash(currentSuccessor)));
        predecessorOfSuccessor = predecessor;
        fingerTable.setSuccessor(predecessor);
    }

    return true;
}

void StabilizeTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    spdlog::get(ownAddress.toString())->debug("Stabilizing network for {}", ownAddress.toString());

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
        this->messagesSent++;
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
    this->messagesSent++;
    messageSent = true;
}

FixFingersTask::FixFingersTask(FingerTable& fingerTable, ConnectionManager& connectionManager, const Poco::Net::SocketAddress& ownAddress) : Task(fingerTable, connectionManager, ownAddress) {
    findTasks.reserve(FingerTable::FINGER_TABLE_SIZE);
    for (std::size_t i = 0; i < FingerTable::FINGER_TABLE_SIZE; ++i) {
        findTasks.push_back(FindTask(fingerTable.getFingerHashValue(i), fingerTable, connectionManager, ownAddress));
    }

    assert(findTasks.size() == FingerTable::FINGER_TABLE_SIZE);
}

void FixFingersTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    spdlog::get(ownAddress.toString())->debug("Fixing fingers");

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

bool FixFingersTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    std::vector<bool> results;
    for (auto& task : findTasks) {
        results.push_back(task.processMessage(from, message));
    }
    return std::any_of(results.begin(), results.end(), [](bool v) { return v; });
}

CheckPredecessorTask::CheckPredecessorTask(FingerTable& fingerTable, ConnectionManager& connectionManager, const Poco::Net::SocketAddress& ownAddress) : Task(fingerTable, connectionManager, ownAddress) {}

void CheckPredecessorTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    spdlog::get(ownAddress.toString())->debug("Checking predecessor");

    state = TaskState::RUNNING;
    auto predecessor = fingerTable.getPredecessor();
    if (predecessor == ownAddress) {
        state = TaskState::FINISHED;
        return;
    }

    auto isEstablished = connectionManager.isConnectionEstablished(predecessor);

    if (!isEstablished) {
        fingerTable.setPredecessor(ownAddress);
        spdlog::get(ownAddress.toString())->debug("Predecessor is not reachable, setting to own address");
        state = TaskState::FINISHED;
        return;
    }

    state = TaskState::FINISHED;
}

void CheckPredecessorTask::update() {}

bool CheckPredecessorTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    (void) from;
    (void) message;
    return true;
}