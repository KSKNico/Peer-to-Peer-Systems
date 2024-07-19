#include "acceptor.hpp"

Acceptor::Acceptor(unsigned int port) : serverSocket(Poco::Net::ServerSocket(port)) {};

std::unique_ptr<Connection> Acceptor::accept() {
    return std::make_unique<Connection>(serverSocket.acceptConnection());
};

bool Acceptor::hasConnection() {
    return serverSocket.poll(0, Poco::Net::Socket::SELECT_READ);
};