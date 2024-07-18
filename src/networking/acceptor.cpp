#include "acceptor.hpp"

Acceptor::Acceptor(unsigned int port) : serverSocket(Poco::Net::ServerSocket(port)) {};

Connection Acceptor::accept() {
    return Connection(serverSocket.acceptConnection());
};