#include "connection.hpp"

#include <cassert>

#include "../globalDefinitions.hpp"
#include "../hash.hpp"

Connection::Connection(Poco::Net::StreamSocket&& socket)
    : socket(socket), inputBuffer(MAX_MESSAGE_SIZE), outputBuffer(MAX_MESSAGE_SIZE), stream(socket) {};

void Connection::sendMessage(Message message) {
    std::string str = message.toString();
    stream.write(str.c_str(), str.size());
};

std::optional<Message> Connection::receiveMessage() {
    std::string str;
    stream.getline(inputBuffer.begin(), MAX_MESSAGE_SIZE - inputBuffer.used());

    // check for eofbit
    if (stream.eof() || stream.fail()) {
        assert(*(inputBuffer.begin() + inputBuffer.used() - 1) != MESSAGE_TERMINATOR);
        return std::nullopt;
    }

    assert(inputBuffer.used() < MAX_MESSAGE_SIZE);
    assert(inputBuffer.used() > 0);
    assert(inputBuffer[inputBuffer.used() - 1] == MESSAGE_TERMINATOR);

    std::string head = Message::extractHead(inputBuffer);
    MessageType type = Message::getMessageType(head);

    std::string content = std::string(inputBuffer.begin(), inputBuffer.begin() + inputBuffer.used() - 1);
    inputBuffer.drain();

    switch (type) {
        case MessageType::ID:
            return IDMessage::fromString(content);
            /*
        case MessageType::JOIN:
            return JoinMessage::fromString(content);
        */
        case MessageType::UNKNOWN:
            return std::nullopt;
        default:
            return std::nullopt;
    }

    return std::nullopt;
};

bool Connection::isConnected() {
    int value = 0;
    socket.getOption(SOL_SOCKET, SO_ERROR, value);
    return value == 0;
};