#include "message.hpp"

Message::Message(MessageData data) : data(data) {}

Message Message::fromBuffer(const Poco::Buffer<char> &buffer) {
    MessageData data;
    std::copy(buffer.begin(), buffer.end(), data.begin());
    return Message(data);
}

Poco::Buffer<char> Message::toBuffer(const Message &message) {
    Poco::Buffer<char> buffer(Message::FIXED_MESSAGE_SIZE);
    std::copy(message.data.begin(), message.data.end(), buffer.begin());
    return buffer;
}