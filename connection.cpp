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

void Connection::sendData(std::vector<std::byte> data) {
    send(this->socket, data.data(), data.size(), 0);
}

std::vector<std::byte> Connection::receiveData() {
    std::vector<std::byte> buffer(1024);
    int bytesRead = recv(this->socket, buffer.data(), buffer.size(), 0);
    buffer.resize(bytesRead);
    return buffer;
}