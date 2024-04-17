#include "connection.hpp"

Connection::Connection(int socket, struct sockaddr_in address)
{
    this->socket = socket;
    this->address = address;
}

Connection::~Connection()
{
    close(this->socket);
}

void Connection::sendData(std::string data) {
    send(this->socket, data.c_str(), data.size(), 0);
}

std::string Connection::receiveData() {
    char buffer[1024] = {0};
    int valread = read(this->socket, buffer, 1024);
    return std::string(buffer);
}