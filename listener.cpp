#include "listener.hpp"

Listener::Listener(uint16_t port) {
    // creates the socket
    this->serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating server socket" << std::endl;
        exit(1);
    }
    

    // binds the socket to an interface and a port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // Port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface 

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        exit(1);
    }

    // listens for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        close(serverSocket);
        exit(1);
    }
}

Listener::~Listener() {
    // close the socket
    close(serverSocket);
}


Connection Listener::acceptConnection() const {
    // accepts a connection
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Error accepting connection" << std::endl;
        close(serverSocket);
        exit(1);
    }

    // creates a connection object
    Connection connection(clientSocket, clientAddress);
    return connection;
}

int Listener::getSocket() const {
    return serverSocket;
}


bool Listener::pollForConnection() const {
    pollfd pollStruct = pollfd();
    pollStruct.fd = serverSocket;
    pollStruct.events = POLLIN;
    pollStruct.revents = 0;

    poll(&pollStruct, 1, -1);

    if (pollStruct.revents & POLLIN)
    {
        return true;
    }

    return false;
}