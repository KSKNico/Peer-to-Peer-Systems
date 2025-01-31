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
        spdlog::get(ownAddress.toString())->debug("Message {}", message->toString());
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
    std::cout << "Finger Table for " << this->ownAddress << std::endl;
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