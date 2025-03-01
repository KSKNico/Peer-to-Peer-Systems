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
    } else if (head == FindResponseMessage::head) {
        return MessageType::FINDR;
    } else if (head == SetPredecessorMessage::head) {
        return MessageType::SPRE;
    } else if (head == SetSuccessorMessage::head) {
        return MessageType::SSUC;
    } else if (head == GetSuccessorMessage::head) {
        return MessageType::GSUC;
    } else if (head == GetSuccessorResponseMessage::head) {
        return MessageType::GSUCR;
    } else if (head == GetPredecessorMessage::head) {
        return MessageType::GPRE;
    } else if (head == GetPredecessorResponseMessage::head) {
        return MessageType::GPRER;
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

FindMessage::FindMessage(const Hash &target, const Poco::Net::SocketAddress& origin) : target(target), origin(origin) {
    type = MessageType::FIND;
}

std::string FindMessage::toString() const {
    return head + MESSAGE_DELIMITER + target.toString() + MESSAGE_DELIMITER + origin.toString();
}

// message looks like this "FIND,<target>,<origin>"
FindMessage FindMessage::fromString(const std::string &str) {
    Hash target = Hash::fromString(str.substr(str.find(MESSAGE_DELIMITER) + 1));
    Poco::Net::SocketAddress origin = Poco::Net::SocketAddress(str.substr(str.rfind(MESSAGE_DELIMITER) + 1));
    return FindMessage(target, origin);
}

FindResponseMessage::FindResponseMessage(const Hash &target) : target(target) {
    type = MessageType::FINDR;
}

std::string FindResponseMessage::toString() const {
    return head + MESSAGE_DELIMITER + target.toString();
}

// message looks like this "FINDR,<target>"
FindResponseMessage FindResponseMessage::fromString(const std::string &str) {
    auto target = Hash::fromString(str.substr(str.find(MESSAGE_DELIMITER) + 1));

    return FindResponseMessage(target);
}

GetSuccessorMessage GetSuccessorMessage::fromString(const std::string &str) {
    assert(str == "GSUC");
    return GetSuccessorMessage();
}

std::string GetSuccessorMessage::toString() const {
    return head;
}

GetSuccessorResponseMessage::GetSuccessorResponseMessage(const Poco::Net::SocketAddress &successor)
    : successor(successor) {
    type = MessageType::GSUCR;
}

std::string GetSuccessorResponseMessage::toString() const {
    return head + MESSAGE_DELIMITER + successor.toString();
}

GetSuccessorResponseMessage GetSuccessorResponseMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);
    std::string address = str.substr(delimiter + 1);
    return GetSuccessorResponseMessage(Poco::Net::SocketAddress(address));
}

GetPredecessorMessage GetPredecessorMessage::fromString(const std::string &str) {
    assert(str == "GPRE");
    return GetPredecessorMessage();
}

std::string GetPredecessorMessage::toString() const {
    return head;
}

GetPredecessorResponseMessage::GetPredecessorResponseMessage(const Poco::Net::SocketAddress &predecessor)
    : predecessor(predecessor) {
    type = MessageType::GPRER;
}

std::string GetPredecessorResponseMessage::toString() const {
    return head + MESSAGE_DELIMITER + predecessor.toString();
}

GetPredecessorResponseMessage GetPredecessorResponseMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);
    std::string address = str.substr(delimiter + 1);
    return GetPredecessorResponseMessage(Poco::Net::SocketAddress(address));
}

Poco::Net::SocketAddress GetPredecessorResponseMessage::getPredecessor() const {
    return predecessor;
}

SetSuccessorMessage::SetSuccessorMessage(const Poco::Net::SocketAddress &successor)
    : successor(successor) {
    type = MessageType::SSUC;
}

std::string SetSuccessorMessage::toString() const {
    return head + MESSAGE_DELIMITER + successor.toString();
}

SetSuccessorMessage SetSuccessorMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);
    std::string address = str.substr(delimiter + 1);
    return SetSuccessorMessage(Poco::Net::SocketAddress(address));
}

SetPredecessorMessage::SetPredecessorMessage(const Poco::Net::SocketAddress &newPredecessor)
    : newPredecessor(newPredecessor) {
    type = MessageType::SPRE;
}

std::string SetPredecessorMessage::toString() const {
    return head + MESSAGE_DELIMITER + newPredecessor.toString();
}

SetPredecessorMessage SetPredecessorMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);
    std::string address = str.substr(delimiter + 1);
    return SetPredecessorMessage(Poco::Net::SocketAddress(address));
}

Poco::Net::SocketAddress SetPredecessorMessage::getNewPredecessor() const {
    return newPredecessor;
}
