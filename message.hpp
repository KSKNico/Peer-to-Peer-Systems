#pragma once
#include <string>
#include <array>
#include <cstdint>

#include "Poco/Buffer.h"

// a message is ascii encoded and is always 1024 bytes long
class Message {
public:
    enum class MessageType {
        GET,
        PUT,
        JOIN,
    };
    constexpr static int FIXED_MESSAGE_SIZE = 1024;

    // used inside of a message to separate parts
    constexpr static char PART_DELIMITER = ' ';

    using MessageData = std::array<char, FIXED_MESSAGE_SIZE>;

    Message(MessageData data);
    MessageType type;
    MessageData data;

    bool isEmpty() const;

    MessageData serialize();
    static Message deserialize(MessageData data);

    static Message fromBuffer(const Poco::Buffer<char> &buffer);
    static Poco::Buffer<char> toBuffer(const Message &message);
    

};