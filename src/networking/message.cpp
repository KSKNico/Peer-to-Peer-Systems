#include "message.hpp"

#include <cassert>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <vector>

#include "../globalDefinitions.hpp"

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

std::string Message::extractHead(const std::string &str) {
    std::string head;
    for (std::size_t i = 0; i < str.size(); i++) {
        if (str[i] == MESSAGE_DELIMITER) {
            break;
        }
        head += str[i];
    }
    return head;
}

MessageType Message::getMessageTypeFromString(const std::string &str) {
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
    } else if (head == FindMessage::head) {
        return MessageType::FIND;
    } else {
        throw std::runtime_error("Unknown message type: " + head);
    }
}

std::string Message::messageTypeToString(const MessageType type) {
    switch (type) {
        case MessageType::ID:
            return "ID";
        case MessageType::JOIN:
            return "JOIN";
        case MessageType::FIND:
            return "FIND";
        case MessageType::INCOMPLETE:
            return "INCOMPLETE";
        case MessageType::ERRORED:
            return "ERROR";
        default:
            throw std::runtime_error("Unknown message type");
    }
    throw std::runtime_error("Unknown message type");
}

std::string Message::toString() const {
    return "";
}

MessageType Message::getType() const {
    return type;
}

bool Message::isComplete() const {
    return type != MessageType::INCOMPLETE;
}

bool Message::isErrored() const {
    return type == MessageType::ERRORED;
}

IDMessage::IDMessage(Poco::Net::SocketAddress ownAddress) : ownAddress(ownAddress) {
    type = MessageType::ID;
}

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

Poco::Net::SocketAddress IDMessage::getOwnAddress() const {
    return ownAddress;
}

IncompleteMessage::IncompleteMessage(const std::string &snippet) : snippet(snippet) {
    type = MessageType::INCOMPLETE;
}

std::string IncompleteMessage::toString() const {
    return snippet;
}

ErroredMessage::ErroredMessage() {
    type = MessageType::ERRORED;
}

std::string ErroredMessage::toString() const {
    return "";
}

FindMessage::FindMessage(const Hash &target, bool isResponse) : target(target), isResponse(isResponse) {
    type = MessageType::JOIN;
}

std::string FindMessage::toString() const {
    return head + MESSAGE_DELIMITER + (isResponse ? "R" : "Q") + MESSAGE_DELIMITER + target.toString() + MESSAGE_DELIMITER;
}

FindMessage FindMessage::fromString(const std::string &str) {
    assert(str[4] == 'Q' || str[4] == 'R');
    bool isResponse = str[4] == 'R';
    auto targetStr = str.substr(6);
    Hash target = Hash::fromString(targetStr);
    return FindMessage(target, isResponse);
}