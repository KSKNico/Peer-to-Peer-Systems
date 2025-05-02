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

std::unique_ptr<Message> Message::fromString(const std::string &str) {
    MessageType type = getMessageTypeFromString(str);

    switch(type) {
        case MessageType::ID:
            return std::make_unique<IDMessage>(IDMessage::fromString(str));
        case MessageType::JOIN:
            return std::make_unique<JoinMessage>(JoinMessage::fromString(str));
        case MessageType::FIND:
            return std::make_unique<FindMessage>(FindMessage::fromString(str));
        case MessageType::FINDR:
            return std::make_unique<FindResponseMessage>(FindResponseMessage::fromString(str));
        case MessageType::GSUC:
            return std::make_unique<GetSuccessorMessage>(GetSuccessorMessage::fromString(str));
        case MessageType::GSUCR:
            return std::make_unique<GetSuccessorResponseMessage>(GetSuccessorResponseMessage::fromString(str));
        case MessageType::SPRE:
            return std::make_unique<SetPredecessorMessage>(SetPredecessorMessage::fromString(str));
        case MessageType::SSUC:
            return std::make_unique<SetSuccessorMessage>(SetSuccessorMessage::fromString(str));
        case MessageType::GPRE:
            return std::make_unique<GetPredecessorMessage>(GetPredecessorMessage::fromString(str));
        case MessageType::GPRER:
            return std::make_unique<GetPredecessorResponseMessage>(GetPredecessorResponseMessage::fromString(str));
        case MessageType::STORE:
            return std::make_unique<StoreMessage>(StoreMessage::fromString(str));
        case MessageType::QUERY:
            return std::make_unique<QueryMessage>(QueryMessage::fromString(str));
        case MessageType::QRESP:
            return std::make_unique<QueryResponseMessage>(QueryResponseMessage::fromString(str));
        case MessageType::INCOMPLETE:
            return std::make_unique<IncompleteMessage>(IncompleteMessage(str));
        case MessageType::ERRORED:
            return std::make_unique<ErroredMessage>();
        default:
            throw std::runtime_error("Unknown message type");
            
    }
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
    } else if (head == StoreMessage::head) {
        return MessageType::STORE;
    } else if (head == QueryMessage::head) {
        return MessageType::QUERY;
    } else if (head == QueryResponseMessage::head) {
        return MessageType::QRESP;
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


// MARK: - StoreMessage
StoreMessage::StoreMessage(uint64_t query, const std::vector<uint64_t> &results) : query(query), results(results) {
    type = MessageType::STORE;
}

std::string StoreMessage::toString() const {
    std::stringstream ss;
    ss << head << MESSAGE_DELIMITER << query << MESSAGE_DELIMITER;
    for (const auto &result : results) {
        ss << result << RESULT_DELIMITER;
    }
    return ss.str();
}

StoreMessage StoreMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);
    std::string query = str.substr(delimiter + 1);
    std::size_t resultDelimiter = query.find(MESSAGE_DELIMITER);
    std::string queryStr = query.substr(0, resultDelimiter);
    std::string resultsStr = query.substr(resultDelimiter + 1);
    std::vector<uint64_t> results;
    std::size_t pos = 0;
    while ((pos = resultsStr.find(RESULT_DELIMITER)) != std::string::npos) {
        results.push_back(std::stoull(resultsStr.substr(0, pos)));
        resultsStr.erase(0, pos + 1);
    }
    return StoreMessage(std::stoull(queryStr), results);
}

std::uint64_t StoreMessage::getQuery() const {
    return query;
}

std::vector<uint64_t> StoreMessage::getResults() const {
    return results;
}

// MARK: - QueryMessage
QueryMessage::QueryMessage(uint64_t query) : query(query) {
    type = MessageType::QUERY;
}

std::string QueryMessage::toString() const {
    return head + MESSAGE_DELIMITER + std::to_string(query);
}

QueryMessage QueryMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);
    std::string query = str.substr(delimiter + 1);
    return QueryMessage(std::stoull(query));
}

std::uint64_t QueryMessage::getQuery() const {
    return query;
}

QueryResponseMessage::QueryResponseMessage(std::uint64_t query, const std::vector<std::uint64_t> &results) : 
    query(query), results(results) {
    type = MessageType::QRESP;
}

std::string QueryResponseMessage::toString() const {
    // if the optinals are empty, we need to return just the head
    std::stringstream ss;

    if (!query.has_value() || !results.has_value()) {
        ss << head;
        return ss.str();
    } else {
        ss << head << MESSAGE_DELIMITER << query.value() << MESSAGE_DELIMITER;
        for (const auto &result : results.value()) {
            ss << result << RESULT_DELIMITER;
        }
        return ss.str();
    }      
}

QueryResponseMessage QueryResponseMessage::fromString(const std::string &str) {
    std::size_t delimiter = str.find(MESSAGE_DELIMITER);

    if (delimiter == std::string::npos) {
        // that means the message contains only the head
        return QueryResponseMessage();
    }

    std::string query = str.substr(delimiter + 1);
    std::size_t resultDelimiter = query.find(MESSAGE_DELIMITER);
    std::string queryStr = query.substr(0, resultDelimiter);
    std::string resultsStr = query.substr(resultDelimiter + 1);
    resultContainer results;
    std::size_t pos = 0;
    while ((pos = resultsStr.find(RESULT_DELIMITER)) != std::string::npos) {
        results.push_back(std::stoull(resultsStr.substr(0, pos)));
        resultsStr.erase(0, pos + 1);
    }
    return QueryResponseMessage(std::stoull(queryStr), results);
}

std::optional<resultType> QueryResponseMessage::getQuery() const {
    return query;
}

std::optional<resultContainer> QueryResponseMessage::getResults() const {
    return results;
}

