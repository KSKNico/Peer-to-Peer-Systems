#include "connectionManager.hpp"

ConnectionManager::ConnectionManager(unsigned int port) : acceptor(port) {}

void ConnectionManager::acceptAllConnections() {
    while (acceptor.hasConnection()) {
        auto connection = acceptor.accept();
        // connections.insert({connection.getPeerAddress(), connection});
        throw std::runtime_error("Not implemented");
    }
}