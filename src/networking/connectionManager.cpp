#include "connectionManager.hpp"

#include <tuple>

#include "connection.hpp"

ConnectionManager::ConnectionManager(unsigned int port) : acceptor(port) {}

void ConnectionManager::acceptAllConnections() {
    while (acceptor.hasConnection()) {
        auto connection = acceptor.accept();
        auto addr = connection->getPeerAddress();

        if (!connections.contains(addr)) {
            connections.emplace(connection->getPeerAddress(), std::move(connection));
        } else {
            continue;
        }
    }
}