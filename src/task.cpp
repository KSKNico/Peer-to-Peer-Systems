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
                std::optional<Poco::Net::SocketAddress> initialPeer) : 
                Task(fingerTable, connectionManager, ownAddress), target(target), initialPeer(initialPeer) {}

void FindTask::init() {
    if (state != TaskState::UNINITIALIZED) {
        return;
    }

    spdlog::get(ownAddress.toString())->debug("Finding target {}", target.toString());

    if (!initialPeer.has_value()) {
        sendTo = fingerTable.getClosestPrecedingFinger(target);
    } else {
        // this means there is no initial Peer provided therefore
        // the first peer to query is the peer provided (initialPeer)
        sendTo = initialPeer.value();
    }

    if (sendTo == ownAddress) {
        targetAddress = ownAddress;
        state = TaskState::FINISHED;
        return;
    }
    connectionManager.existsElseConnect(sendTo);

    state = TaskState::RUNNING;
}

void FindTask::update() {
    assert(messagesSent <= 10);

    if (state == TaskState::FINISHED) {
        return;
    }

    if (connectionManager.isConnectionEstablished(sendTo) && messagesSent == 0) {
        connectionManager.sendMessage(sendTo, FindMessage(target, ownAddress));
        this->messagesSent++;
        lastSentTo = sendTo;
        return;
    }

    /*
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
    */
}

bool FindTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    if (state == TaskState::FINISHED) {
        return false;
    }

    // one special case!: where the from is before the target and the target is before the own address
    // the message type is FIND and not FINDR!
    if (message->getType() == MessageType::FIND) {
        // auto findMessage = dynamic_cast<const FindMessage*>(message.get());

        if (Hash(target).isBetween(from, Hash(ownAddress))) {
            targetAddress = ownAddress;
            state = TaskState::FINISHED;
            return true;
        }
    }

    if (message->getType() != MessageType::FINDR) {
        return false;
    }

    auto findResponseMessage = dynamic_cast<const FindResponseMessage*>(message.get());

    if (findResponseMessage->target != target) {
        return false;
    }

    // std::cout << from.toString() << " ---> " << this->ownAddress.toString() << std::endl;
    spdlog::get(ownAddress.toString())->debug("Received find response message");

    targetAddress = from;

    state = TaskState::FINISHED;
    /*
        connectionManager.existsElseConnect(findMessage->referenceAddress);
        targetAddress = findMessage->referenceAddress;

        spdlog::get(ownAddress.toString())->debug("Found target {} at {}", 
        target.toString(), 
        targetAddress.value().toString());
        
        state = TaskState::FINISHED;
    } else {
        nextHop = findMessage->referenceAddress;
    }
    */
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
    fingerTable.updateSuccessor(targetAddressOptional.value());

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
        fingerTable.updateSuccessor(predecessor);
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
        fingerTable.updatePredecessor(ownAddress);
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