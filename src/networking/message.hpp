#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "../hash.hpp"
#include "Poco/Buffer.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Net/SocketAddress.h"

enum class MessageType {
    ID,
    JOIN,
    FIND,
    INCOMPLETE,
    ERRORED,
    UNKNOWN,
};

class Message {
   public:
    virtual ~Message() = default;

    virtual std::string toString() const;
    // static Message fromString(const std::string &str);
    static std::string extractHead(const Poco::FIFOBuffer &buffer);
    static std::string extractHead(const std::string &str);

    static MessageType getMessageTypeFromString(const std::string &str);

    static std::string messageTypeToString(const MessageType type);

    bool isComplete() const;
    bool isErrored() const;

    MessageType getType() const;

   protected:
    MessageType type;
};

// used to exchange the ID of a peer, i.e. the server socket address with which other peers can connect to this peer
// it is also used for the ID in the Chord ring p2p network
class IDMessage : public Message {
   public:
    IDMessage(Poco::Net::SocketAddress);
    std::string toString() const override;
    static IDMessage fromString(const std::string &);

    static constexpr std::string head = "ID";

    Poco::Net::SocketAddress getOwnAddress() const;

    const Poco::Net::SocketAddress ownAddress;
};

class JoinMessage : public Message {
   public:
    JoinMessage(Hash);
    std::string toString() const override;
    static Message fromString(const std::string &);

    static constexpr std::string head = "JOIN";

    const Hash id;
};

class FindMessage : public Message {
   public:
    FindMessage(const Hash &hash, bool isResponse);
    std::string toString() const override;
    static Message fromString(const std::string &);

    static constexpr std::string head = "FIND";

    const Hash target;
    const bool isResponse;
};

class IncompleteMessage : public Message {
   public:
    IncompleteMessage(const std::string &);
    std::string toString() const override;

   private:
    const std::string snippet;
};

class ErroredMessage : public Message {
   public:
    ErroredMessage();
    std::string toString() const override;
};