#include "connectionManager.hpp"

#include <queue>
#include <tuple>

#include "connection.hpp"

ConnectionManager::ConnectionManager(Poco::Net::SocketAddress ownAddress) : ownAddress(ownAddress), acceptor(ownAddress.port()) {}

void ConnectionManager::acceptAllConnections() {
    while (acceptor.hasConnection()) {
        auto connection = acceptor.accept();
        auto addr = connection->getPeerAddress();

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

void ConnectionManager::updateIncomingConnections() {
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
                establishedConnections.insert({idMessage->getOwnAddress(), std::move(connection)});
            } else {
                std::cout << "Failed to cast message to IDMessage" << std::endl;
            }
        } else {
            std::cout << "Received invalid message from incoming connection" << std::endl;
        }
    }
}