#include "connection.hpp"

#include <cassert>

#include "../globalDefinitions.hpp"
#include "../hash.hpp"
#include "spdlog/spdlog.h"

enum class ConnectionState {
    UNINITIALIZED,
    PENDING,
    CONNECTED,
    DISCONNECTED,
};

Connection::Connection(Poco::Net::StreamSocket&& socket)
    : socket(socket), stream(socket) {
    socket.setBlocking(false);
    str.reserve(MAX_MESSAGE_SIZE);
    remaining.reserve(MAX_MESSAGE_SIZE);
};

void Connection::_sendString(const std::string& str) {
    stream.write(str.c_str(), str.size());
    stream.flush();
};

void Connection::sendMessage(const Message& message) {
    assert(message.toString().size() < MAX_MESSAGE_SIZE);

    // call the specialized function for the message type
    std::string str = message.toString();
    str += MESSAGE_TERMINATOR;
    stream.write(str.c_str(), str.size());
    stream.flush();
};

void Connection::sendMessages(const std::vector<Message>& messages) {
    for (auto& message : messages) {
        sendMessage(message);
    }
};

std::unique_ptr<Message> Connection::receiveMessage() {
    str.clear();
    std::getline(stream, str, MESSAGE_TERMINATOR);

    str = remaining + str;

    // check for eofbit
    if (stream.bad() || stream.fail()) {
        stream.clear();
        return std::make_unique<ErroredMessage>();
    }

    if (stream.eof()) {
        assert(str.back() != MESSAGE_TERMINATOR);
        remaining = str;
        stream.clear();
        return std::make_unique<IncompleteMessage>(remaining);
    }

    assert(str.size() > 0);
    assert(str.size() < MAX_MESSAGE_SIZE);
    // spdlog::get(socket.address().toString())->debug("Received message of type {}", head);
    
    return Message::fromString(str);

    // throw std::runtime_error("Unknown message type");
};

bool Connection::isConnected() {
    int value = 0;
    socket.getOption(SOL_SOCKET, SO_ERROR, value);
    return value == 0;
};

bool Connection::isReadable() {
    if (socket.poll(0, Poco::Net::Socket::SELECT_READ)) {
        return true;
    }

    return stream.rdbuf()->in_avail() > 0;
};

bool Connection::isWritable() {
    return socket.poll(0, Poco::Net::Socket::SELECT_WRITE);
};

Poco::Net::SocketAddress Connection::getOwnAddress() const {
    return socket.address();
};

Poco::Net::SocketAddress Connection::getPeerAddress() const {
    return socket.peerAddress();
};