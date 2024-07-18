#include "message.hpp"

#include <cassert>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <vector>

#include "../globalDefinitions.hpp"

Message::Message() {}

std::string Message::extractHead(const Poco::FIFOBuffer &buffer) {
    std::string head;
    for (std::size_t i = 0; i < buffer.used(); i++) {
        if (buffer[i] == MESSAGE_DELIMITER) {
            break;
        }
        head += buffer[i];
    }
    return head;
}

MessageType Message::getMessageType(const std::string &str) {
    std::string head;
    for (std::size_t i = 0; i < str.size(); i++) {
        if (str[i] == MESSAGE_DELIMITER) {
            break;
        }
        head += str[i];
    }

    if (head == IDMessage::head) {
        return MessageType::ID;
    } else if (head == JoinMessage::head) {
        return MessageType::JOIN;
    } else {
        return MessageType::UNKNOWN;
    }
}

std::string Message::messageTypeToString(const MessageType type) {
    switch (type) {
        case MessageType::ID:
            return "ID";
        case MessageType::JOIN:
            return "JOIN";
        case MessageType::UNKNOWN:
            return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string Message::toString() const {
    return "";
}

IDMessage::IDMessage(Poco::Net::SocketAddress ownAddress) : ownAddress(ownAddress) {}

std::string IDMessage::toString() const {
    return head + MESSAGE_DELIMITER + ownAddress.toString();
}

IDMessage IDMessage::fromString(const std::string &str) {
    std::string address;
    for (std::size_t i = 0; i < str.size(); i++) {
        if (str[i] == MESSAGE_DELIMITER) {
            address = str.substr(i + 1);
            break;
        }
    }

    return IDMessage(Poco::Net::SocketAddress(address));
}
