#include "connector.hpp"

Connector::Connector() {}

Connection Connector::connectTo(sockaddr_in address) {
    // socket needs to be unblocking
    int clientSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (clientSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    if (connect(clientSocket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        close(clientSocket);
        throw std::runtime_error("Error connecting to server");
    }

    return Connection(clientSocket, address);
}