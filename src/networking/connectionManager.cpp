#include "connectionManager.hpp"

#include <cassert>
#include <queue>
#include <tuple>

#include "connection.hpp"

ConnectionManager::ConnectionManager(Poco::Net::SocketAddress ownAddress) : ownAddress(ownAddress), acceptor(ownAddress.port()) {}

void ConnectionManager::acceptAllConnections() {
    while (acceptor.hasConnection()) {
        auto connection = acceptor.accept();
        auto addr = connection->getPeerAddress();

        if (connection.get() == nullptr) {
            std::cout << "Failed to accept connection" << std::endl;
            continue;
        }

        if (pendingIncomingConnections.contains(addr) || establishedConnections.contains(addr)) {
            std::cout << "Incoming connection already exists" << std::endl;
        } else {
            pendingIncomingConnections.insert({addr, std::move(connection)});
        }
    }
}

bool ConnectionManager::existsElseConnect(const Poco::Net::SocketAddress &address) {
    if (pendingOutgoingConnections.contains(address) || establishedConnections.contains(address) || pendingIncomingConnections.contains(address)) {
        return true;
    }

    connectTo(address);
    return false;
}

void ConnectionManager::connectTo(const Poco::Net::SocketAddress &address) {
    if (pendingOutgoingConnections.contains(address) || establishedConnections.contains(address)) {
        throw std::runtime_error("Connection already exists");
    }

    auto connection = connector.connect(address, false);
    if (connection == nullptr) {
        throw std::runtime_error("Failed to connect outbound to " + address.toString());
    }

    pendingOutgoingConnections.insert({address, std::move(connection)});
}

bool ConnectionManager::isConnectionEstablished(const Poco::Net::SocketAddress &address) const {
    return establishedConnections.contains(address);
}

void ConnectionManager::updateOutgoingConnections() {
    auto toErase = std::vector<Poco::Net::SocketAddress>();
    for (auto &[addr, connection] : pendingOutgoingConnections) {
        if (connection->isConnected()) {
            connection->sendMessage(IDMessage(ownAddress));
            establishedConnections.insert({addr, std::move(connection)});
            toErase.push_back(addr);
        }
    }

    for (const auto &addr : toErase) {
        pendingOutgoingConnections.erase(addr);
    }
}

void ConnectionManager::sendMessage(const Poco::Net::SocketAddress &address, const Message &message) {
    if (establishedConnections.contains(address)) {
        establishedConnections.at(address)->sendMessage(message);
    } else {
        throw std::runtime_error("Connection does not exist");
    }
}

std::vector<MessagePair> ConnectionManager::receiveMessages() {
    std::vector<MessagePair> messages;
    messages.reserve(establishedConnections.size());
    for (auto &[addr, connection] : establishedConnections) {
        if (!connection->isReadable()) {
            continue;
        }

        std::unique_ptr<Message> message = connection->receiveMessage();
        if (message->isComplete() && !message->isErrored()) {
            messages.push_back({addr, std::move(message)});
        }
    }
    return messages;
}

void ConnectionManager::updateIncomingConnections() {
    std::vector<Poco::Net::SocketAddress> toErase;

    for (auto &[addr, connection] : pendingIncomingConnections) {
        if (!connection->isConnected()) {
            continue;
        }

        if (!connection->isReadable()) {
            continue;
        }

        auto message = connection->receiveMessage();
        if (message->getType() == MessageType::ID) {
            if (auto idMessage = dynamic_cast<IDMessage *>(message.get())) {
                assert(idMessage->getOwnAddress().host() == addr.host());
                establishedConnections.insert({idMessage->getOwnAddress(), std::move(connection)});
                toErase.push_back(addr);
            } else {
                throw std::runtime_error("Received invalid ID message");
            }
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

void ConnectionManager::checkEstablishedConnections() {
    for (auto &[addr, connection] : establishedConnections) {
        if (!connection->isConnected()) {
            establishedConnections.erase(addr);
        }
    }
}

void ConnectionManager::update() {
    acceptAllConnections();
    updateOutgoingConnections();
    updateIncomingConnections();
    checkEstablishedConnections();
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