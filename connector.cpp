#include "connector.hpp"

Connector::Connector() {}

Connection Connector::connectTo(sockaddr_in address) {
    // socket needs to be unblocking
    int clientSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (clientSocket == -1) {
        throw std::runtime_error("Error creating socket");
    }

    connect(clientSocket, (struct sockaddr *)&address, sizeof(address));

    return Connection(clientSocket, address);
}