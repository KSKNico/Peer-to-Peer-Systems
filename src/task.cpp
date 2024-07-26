#include "task.hpp"

#include <cassert>

Task::Task(FingerTable& fingerTable, ConnectionManager& connectionManager) : fingerTable(fingerTable), connectionManager(connectionManager) {
}

FindTask::FindTask(const Hash& target, FingerTable& fingerTable, ConnectionManager& connectionManager) : Task(fingerTable, connectionManager), target(target) {}

void FindTask::init() {
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
        state == TaskState::FINISHED;
    } else {
        nextHop = findMessage->referenceAddress;
    }
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
    connectionManager.existsElseConnect(joinAddress);
}

void JoinTask::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
}