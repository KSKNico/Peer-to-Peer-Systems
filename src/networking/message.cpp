#include "message.hpp"

#include <cassert>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <vector>

#include "../globalDefinitions.hpp"

/*
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
*/

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
        case MessageType::FINDR:
            return "FINDR";
        case MessageType::INCOMPLETE:
            return "INCOMPLETE";
        case MessageType::ERRORED:
            return "ERROR";
        default:
            throw std::runtime_error("Unknown message type");
    }
    throw std::runtime_error("Unknown message type");
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

JoinMessage::JoinMessage() {
    type = MessageType::JOIN;
}

JoinMessage JoinMessage::fromString(const std::string &str) {
    assert(str == "JOIN");
    return JoinMessage();
}

std::string JoinMessage::toString() const {
    return head;
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

FindMessage::FindMessage(const Hash &target) : target(target) {
    type = MessageType::JOIN;
}

std::string FindMessage::toString() const {
    return head + MESSAGE_DELIMITER + target.toString();
}

FindMessage FindMessage::fromString(const std::string &str) {
    Hash target = Hash::fromString(str.substr(str.find(MESSAGE_DELIMITER) + 1));
    return FindMessage(target);
}

FindResponseMessage::FindResponseMessage(const Hash &target, const Poco::Net::SocketAddress &addr) : referenceAddress(addr), target(target) {
    type = MessageType::FINDR;
}

std::string FindResponseMessage::toString() const {
    return head + MESSAGE_DELIMITER + target.toString() + MESSAGE_DELIMITER + referenceAddress.toString();
}

// message looks like this "FINDR,<target>,<address>"
FindResponseMessage FindResponseMessage::fromString(const std::string &str) {
    std::size_t firstDelimiter = str.find(MESSAGE_DELIMITER);
    std::size_t secondDelimiter = str.find(MESSAGE_DELIMITER, firstDelimiter + 1);

    std::string target = str.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1);
    std::string address = str.substr(secondDelimiter + 1);

    return FindResponseMessage(Hash::fromString(target), Poco::Net::SocketAddress(address));
}