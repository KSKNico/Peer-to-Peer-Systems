#include "connectionManager.hpp"

#include <cassert>
#include <queue>
#include <tuple>

#include "connection.hpp"
#include "spdlog/spdlog.h"

ConnectionManager::ConnectionManager(const Poco::Net::SocketAddress ownAddress) : ownAddress(ownAddress), acceptor(ownAddress.port()) {
    // spdlog::get(ownAddress.toString())->debug("ConnectionManager created with own address: {}", this->ownAddress.toString());
}

void ConnectionManager::acceptAllConnections() {
    while (acceptor.hasConnection()) {
        auto connection = acceptor.accept();
        auto addr = connection->getPeerAddress();

        assert(ownAddress != connection->getPeerAddress()); 

        if (connection.get() == nullptr) {
            spdlog::get(ownAddress.toString())->error("Failed to accept connection");
            continue;
        }

        if (pendingIncomingConnections.contains(addr) || establishedConnections.contains(addr)) {
            spdlog::get(ownAddress.toString())->warn("Incoming connection already exists");
        } else {
            pendingIncomingConnections.insert(std::make_pair(addr, std::make_pair(std::move(connection), Timing())));
        }
    }
}

bool ConnectionManager::existsElseConnect(const Poco::Net::SocketAddress &address) {
    if (pendingOutgoingConnections.contains(address) || 
        establishedConnections.contains(address) || 
        pendingIncomingConnections.contains(address)) {
        return true;
    }

    connectTo(address);
    return false;
}

void ConnectionManager::connectTo(const Poco::Net::SocketAddress &address) {
    assert(address != ownAddress);

    if (pendingOutgoingConnections.contains(address) || establishedConnections.contains(address)) {
        throw std::runtime_error("Connection already exists");
    }

    auto connection = connector.connect(address, false);
    if (connection == nullptr) {
        throw std::runtime_error("Failed to connect outbound to " + address.toString());
    }

    spdlog::get(ownAddress.toString())->debug("Opening connection to {}", address.toString());
    pendingOutgoingConnections.insert(std::make_pair(address, std::make_pair(std::move(connection), Timing())));
}

bool ConnectionManager::isConnectionEstablished(const Poco::Net::SocketAddress &address) const {
    return establishedConnections.contains(address);
}

void ConnectionManager::connectToAndSend(const Poco::Net::SocketAddress &address, 
    std::unique_ptr<Message> message) {
    messageBuffers.insert({address, std::move(message)});

    if (pendingOutgoingConnections.contains(address) || establishedConnections.contains(address)) {
        return;
    }

    connectTo(address);

    return;
}

void ConnectionManager::updateOutgoingConnections() {
    auto toErase = std::vector<Poco::Net::SocketAddress>();
    for (auto &[addr, connectionPair] : pendingOutgoingConnections) {
        auto &[connection, timing] = connectionPair;
        if (!connection->isConnected()) {
            continue;
        }
        connection->sendMessage(IDMessage(ownAddress));
        timing.update();
        establishedConnections.insert(std::make_pair(addr, std::make_pair(std::move(connection), timing)));
        toErase.push_back(addr);
        spdlog::get(ownAddress.toString())->debug("Sent outgoing ID message to {}", addr.toString());
    }

    for (const auto &addr : toErase) {
        pendingOutgoingConnections.erase(addr);
    }
}

void ConnectionManager::sendMessage(const Poco::Net::SocketAddress &address, const Message &message) {
    if (establishedConnections.contains(address)) {
        establishedConnections.at(address).first->sendMessage(message);
        establishedConnections.at(address).second.update();
        spdlog::get(ownAddress.toString())->debug("{} message sent to {}",
        Message::extractHead(message.toString()),
        address.toString());
    } else {
        for (auto &c : this->getEstablishedConnections()) {
            spdlog::get(ownAddress.toString())->error("Established connection: {}", c.toString());
        }
        throw std::runtime_error("Connection from " + ownAddress.toString() + " does not exist to " 
        + address.toString());
    }
}

std::vector<MessagePair> ConnectionManager::receiveMessages() {
    std::vector<MessagePair> messages;
    messages.reserve(establishedConnections.size());
    for (auto &[addr, connectionPair] : establishedConnections) {
        auto &[connection, timing] = connectionPair;
        if (!connection->isReadable()) {
            continue;
        }

        std::unique_ptr<Message> message = connection->receiveMessage();
        spdlog::get(ownAddress.toString())->debug("Received {} message from {}", 
        Message::extractHead(message->toString()),
        addr.toString());

        if (message->isComplete() && !message->isErrored()) {
            messages.push_back({addr, std::move(message)});
            timing.update();
        }
    }
    return messages;
}

void ConnectionManager::updateIncomingConnections() {
    std::vector<Poco::Net::SocketAddress> toErase;

    for (auto &[addr, connectionPair] : pendingIncomingConnections) {
        auto &[connection, timing] = connectionPair;
        if (!connection->isConnected()) {
            continue;
        }

        if (!connection->isReadable()) {
            continue;
        }

        auto message = connection->receiveMessage();
        if (message->getType() == MessageType::ID) {
            auto idMessage = dynamic_cast<IDMessage *>(message.get());

            if (idMessage == nullptr) {
                throw std::runtime_error("Received invalid ID message");
            }

            if (establishedConnections.contains(idMessage->getOwnAddress())) {
                spdlog::get(ownAddress.toString())->warn("Incoming connection already exists");
                toErase.push_back(addr);
                continue;
            }

            if (idMessage->getOwnAddress() == ownAddress) {
                throw std::runtime_error("Received connection from self");
            }

            assert(idMessage->getOwnAddress().host() == addr.host());

            timing.update();
            establishedConnections.insert(std::make_pair(idMessage->getOwnAddress(), 
            std::make_pair(std::move(connection), timing)));
            toErase.push_back(addr);
            spdlog::get(ownAddress.toString())->debug("Incoming connection from {} established (remote peer server socket IP: {})", 
                        addr.toString(),
                        idMessage->getOwnAddress().toString());
        } else if (message->getType() == MessageType::INCOMPLETE) {
            continue;
        } else {
            throw std::runtime_error("Received invalid message type");
        }
    }

    for (const auto &addr : toErase) {
        pendingIncomingConnections.erase(addr);
    }
}

std::vector<Poco::Net::SocketAddress> ConnectionManager::checkEstablishedConnections() {
    std::vector<Poco::Net::SocketAddress> erased;
    for (auto &[addr, connectionPair] : establishedConnections) {
        auto &[connection, _] = connectionPair;
        if (!connection->isConnected()) {
            erased.push_back(addr);
            establishedConnections.erase(addr);
        }
    }

    return erased;
}

void ConnectionManager::sendAllBufferedMessages() {
    for (auto &[addr, message] : messageBuffers) {
        if (!establishedConnections.contains(addr)) {
            continue;
        }
        sendMessage(addr, *message);
    }
    messageBuffers.clear();
}

std::vector<Poco::Net::SocketAddress> ConnectionManager::update() {
    acceptAllConnections();
    updateOutgoingConnections();
    updateIncomingConnections();
    sendAllBufferedMessages();
    return checkEstablishedConnections();
}

void ConnectionManager::closeConnection(const Poco::Net::SocketAddress &address) {
    establishedConnections.erase(address);
}

std::size_t ConnectionManager::getEstablishedConnectionsCount() const {
    return establishedConnections.size();
}

std::vector<Poco::Net::SocketAddress> ConnectionManager::getEstablishedConnections() const {
    std::vector<Poco::Net::SocketAddress> addresses;
    addresses.reserve(establishedConnections.size());
    for (const auto &[addr, _] : establishedConnections) {
        addresses.push_back(addr);
    }
    return addresses;
}