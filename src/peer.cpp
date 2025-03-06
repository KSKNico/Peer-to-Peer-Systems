#include "peer.hpp"

#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Poco/Net/SocketReactor.h"
#include "resultHandler.hpp"

Peer::Peer(const Poco::Net::SocketAddress& ownAddress,
           const Poco::Net::SocketAddress& remoteAddress,
           const spdlog::level::level_enum logLevel) : ownAddress(ownAddress),
                                                       connectionManager(ownAddress),
                                                       fingerTable(ownAddress),
                                                       taskManager(ownAddress, connectionManager, fingerTable) {
                                                    
    createLogger(logLevel);
    spdlog::get(ownAddress.toString())->set_level(logLevel);
    spdlog::get(ownAddress.toString())->info("Starting peer at {} with log level {}", ownAddress.toString(), (int) logLevel);
    auto joinTask = std::make_unique<JoinTask>(remoteAddress, fingerTable, connectionManager, ownAddress);
    taskManager.addTask(std::move(joinTask));
    
}

Peer::Peer(const Poco::Net::SocketAddress& ownAddress,
           const spdlog::level::level_enum logLevel) : ownAddress(ownAddress),
                                                       connectionManager(ownAddress),
                                                       fingerTable(ownAddress),
                                                       taskManager(ownAddress, connectionManager, fingerTable) {
    createLogger(logLevel);
    spdlog::get(ownAddress.toString())->set_level(logLevel);
    spdlog::get(ownAddress.toString())->info("Starting bootstrapping peer at {} with log level {}", ownAddress.toString(), (int) logLevel);
}

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
        // spdlog::get(ownAddress.toString())->debug("Message {}", message->toString());
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

            spdlog::get(ownAddress.toString())->debug("Received FIND message from {} with target {} and origin {}", 
            from.toString(), 
            findMessage->target.toString(), 
            findMessage->origin.toString());

            
            if (fingerTable.getPredecessor() == ownAddress || fingerTable.getSuccessor() == ownAddress) {
                connectionManager.connectToAndSend(findMessage->origin, 
                std::make_unique<FindResponseMessage>(findMessage->target));
                return;
            }

            // we have no redirecting or answering to do if the target is between the from and the own address
            // because this peer is responsible for the target
            if (findMessage->target.isBetween(Hash(from), Hash(ownAddress))) {
                // do nothing
                return;
            }

            // the FIND is looking for this peer
            if (findMessage->target == Hash(ownAddress)) {
                connectionManager.connectToAndSend(findMessage->origin, 
                std::make_unique<FindResponseMessage>(findMessage->target));
                return;
            }

            // the FIND is looking for this peer
            if (findMessage->target.isBetween(Hash(fingerTable.getPredecessor()), Hash(ownAddress))) {
                spdlog::get(ownAddress.toString())->debug("Sending FIND to origin {}", findMessage->origin.toString());
                connectionManager.connectToAndSend(findMessage->origin, 
                std::make_unique<FindResponseMessage>(findMessage->target));
                return;
            }
    
            // another peer can be queried
            next = fingerTable.getClosestPrecedingFinger(findMessage->target);
            spdlog::get(ownAddress.toString())->debug("Forwarding FIND message to {}", next.toString());

            connectionManager.sendMessage(next, *findMessage);
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
            fingerTable.updatePredecessor(setPredecessorMessage->getNewPredecessor());
            return;
        default:
            return;
    }
}

void Peer::printConnections() const {
    std::cout << "Current Peer: " << this->ownAddress
              << " This ID: " << Hash(this->ownAddress).toString()
              << std::endl;
    std::cout << "------" << std::endl;
    for (const auto& addr : connectionManager.getEstablishedConnections()) {
        std::cout << addr.toString() << "\t" << Hash(addr).toString() << std::endl;
    }
    std::cout << "------" << std::endl
              << std::endl;
}

void Peer::printFingerTable() const {
    Poco::Net::SocketAddress lastFinger;
    std::cout << "Finger Table for " << this->ownAddress << " (" 
    << Hash::hashSocketAddress(this->ownAddress).toString() << ")" << std::endl;
    std::cout << "This successor " << fingerTable.getSuccessor() << std::endl;
    std::cout << "This predecessor " << fingerTable.getPredecessor() << std::endl;
    std::cout << "------" << std::endl;
    lastFinger = this->fingerTable.getFinger(0);
    std::cout << "0\t" << lastFinger << std::endl;
    for (std::size_t i = 0; i < FingerTable::FINGER_TABLE_SIZE; ++i) {
        if (lastFinger == this->fingerTable.getFinger(i)) {
            continue;
        }
        std::cout << i << "\t" << this->fingerTable.getFinger(i) << std::endl;
        lastFinger = this->fingerTable.getFinger(i);
    }
    std::cout << "------" << std::endl
              << std::endl;
}

Poco::Net::SocketAddress Peer::getSuccessor() const {
    return fingerTable.getSuccessor();
}

Poco::Net::SocketAddress Peer::getPredecessor() const {
    return fingerTable.getPredecessor();
}

std::size_t Peer::getConnectionsCount() const {
    return connectionManager.getEstablishedConnectionsCount();
}

void Peer::createLogger(const spdlog::level::level_enum logLevel) {
    sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    logger = std::make_shared<spdlog::logger>(ownAddress.toString(), sink);
    logger->set_level(logLevel);
    spdlog::register_logger(logger);
}