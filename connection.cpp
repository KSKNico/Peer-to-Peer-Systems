#include "connection.hpp"

Connection::Connection(int socket, struct sockaddr_in address)
{
    this->socket = socket;
    this->address = address;
    this->buffer = std::array<std::byte, bufferSize>();
    this->offset = 0;
}

Connection::~Connection()
{
    // close(this->socket);
}

bool Connection::isWritable() const {
    struct pollfd pfd;
    pfd.fd = socket;
    pfd.events = POLLOUT;

    if (poll(&pfd, 1, -1) == 1) {
        return pfd.revents & POLLOUT;
    } else {
        throw std::runtime_error("Error polling socket");
    }
}

bool Connection::isValid() const {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &len) != 0) {
        throw std::runtime_error("Error getting socket options");
    }

    if (error != 0) {
        return false;
    }

    return true;
}

bool Connection::hasData() const {
    struct pollfd pfd;
    pfd.fd = socket;
    pfd.events = POLLIN;

    if (poll(&pfd, 1, -1) == 1) {
        return pfd.revents & POLLIN;
    } else {
        throw std::runtime_error("Error polling socket");
    }
}

void Connection::sendData(std::vector<std::byte> data) const {
    send(this->socket, data.data(), data.size(), 0);
}

int Connection::receiveData() {
    int r = recv(this->socket, this->buffer.data() + this->offset, bufferSize - this->offset, 0);
    this->offset += r;
    return r;
}

sockaddr_in Connection::getAddress() const {
    return this->address;
}

int Connection::getSocket() const {
    return this->socket;
}

std::optional<Message> Connection::readMessage() {
    int count = this->receiveData();
    
    auto message = std::optional<Message>(); 
    if (count < 4 && firstPart) {
        return message;
    }

    // now we can deserialize
    // get the length of the message
    if (firstPart) {
        uint32_t length = 0;
        length |= std::to_integer<uint32_t>(this->buffer[0]);
        length |= std::to_integer<uint32_t>(this->buffer[1]) << 8;
        length |= std::to_integer<uint32_t>(this->buffer[2]) << 16;
        length |= std::to_integer<uint32_t>(this->buffer[3]) << 24;
        remaining = length;
    }

    if (remaining < count) {
        // this means we haven't received the entire message
        firstPart = false;
        remaining = length - count;
        return message;
    }

    // we have the entire message, deserialize it
    std::vector<std::byte> messageData(this->buffer.begin(), this->buffer.begin() + length);
    message = Message::deserialize(messageData);

    // shift the buffer to the left
    std::copy(this->buffer.begin() + length, this->buffer.end(), this->buffer.begin());
    offset = 0;

    firstPart = true;

    return message;
}

void Connection::writeMessage(Message message) const {
    auto data = message.serialize();
    sendData(data);
}