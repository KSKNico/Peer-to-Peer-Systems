#include "peer.hpp"

#include <iostream>

#include "Poco/Net/SocketReactor.h"
#include "resultHandler.hpp"

Peer::Peer(const Poco::Net::SocketAddress& ownAddress,
           const Poco::Net::SocketAddress& remoteAddress) : ownAddress(ownAddress),
                                                            connectionManager(ownAddress),
                                                            fingerTable(ownAddress),
                                                            taskManager(ownAddress, connectionManager, fingerTable) {
    auto joinTask = std::make_unique<JoinTask>(ownAddress, remoteAddress, fingerTable, connectionManager);
    taskManager.addTask(std::move(joinTask));
}

Peer::Peer(const Poco::Net::SocketAddress& ownAddress) : ownAddress(ownAddress),
                                                         connectionManager(ownAddress),
                                                         fingerTable(ownAddress),
                                                         taskManager(ownAddress, connectionManager, fingerTable) {}

void Peer::update() {
    auto removed = connectionManager.update();

    // the removed connections should also be removed from the finger table
    for (auto& address : removed) {
        fingerTable.removeAddress(address);
    }

    taskManager.update();

    auto messagePairs = connectionManager.receiveMessages();
    for (auto& [from, message] : messagePairs) {
        taskManager.processMessage(from, message);
        this->processMessage(from, message);
    }
}

void Peer::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    auto msgType = message->getType();
    SetPredecessorMessage* setPredecessorMessage;
    FindMessage* findMessage;
    Poco::Net::SocketAddress next;
    switch (msgType) {
        case MessageType::FIND:
            findMessage = dynamic_cast<FindMessage*>(message.get());
            if (Hash(findMessage->target).isBetween(Hash(from), Hash(ownAddress))) {
                connectionManager.sendMessage(from, FindResponseMessage(findMessage->target, ownAddress));
            } else {
                next = fingerTable.getClosestPrecedingFinger(findMessage->target);
                connectionManager.sendMessage(from, FindResponseMessage(findMessage->target, next));
            }
            return;
        case MessageType::GSUC:
            connectionManager.sendMessage(from, GetSuccessorResponseMessage(fingerTable.getSuccessor()));
            return;
        case MessageType::GPRE:
            connectionManager.sendMessage(from, GetPredecessorResponseMessage(fingerTable.getPredecessor()));
            return;
        case MessageType::SSUC:
            throw std::runtime_error("Not implemented");
            return;
        case MessageType::SPRE:
            setPredecessorMessage = dynamic_cast<SetPredecessorMessage*>(message.get());
            fingerTable.setPredecessor(setPredecessorMessage->getNewPredecessor());
            return;
        default:
            return;
    }
}

void Peer::printConnections() const {
    std::cout << "Current Peer: " << this->ownAddress << " This ID: " << Hash(this->ownAddress).toString() << std::endl;
    std::cout << "------" << std::endl;
    for (const auto& addr : connectionManager.getEstablishedConnections()) {
        std::cout << addr.toString() << "\t" << Hash(addr).toString() << std::endl;
    }
    std::cout << "------" << std::endl
              << std::endl;
}

std::size_t Peer::getConnectionsCount() const {
    return connectionManager.getEstablishedConnectionsCount();
}
