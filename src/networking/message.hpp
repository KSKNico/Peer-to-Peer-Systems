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
    INCOMPLETE,
    ERROR,
    UNKNOWN
};

class Message {
   public:
    virtual std::string toString() const;
    // static Message fromString(const std::string &str);
    static std::string extractHead(const Poco::FIFOBuffer &buffer);
    static std::string extractHead(const std::string &str);

    static MessageType getMessageType(const std::string &str);
    static MessageType getMessageType(const Message &message);

    static std::string messageTypeToString(const MessageType type);

    bool isComplete() const;
    bool isError() const;

   protected:
    MessageType type;
};

// used to exchange the id of a peer
class IDMessage : public Message {
   public:
    IDMessage(Poco::Net::SocketAddress);
    std::string toString() const override;
    static IDMessage fromString(const std::string &);
    static constexpr std::string head = "ID";

   private:
    Poco::Net::SocketAddress ownAddress;
};

class JoinMessage : public Message {
   public:
    JoinMessage(Hash);
    std::string toString() const override;
    static Message fromString(const std::string &);
    static constexpr std::string head = "JOIN";

   private:
    Hash id;
};

class IncompleteMessage : public Message {
   public:
    IncompleteMessage(const std::string &);
    std::string toString() const override;

   private:
    const std::string snippet;
};

class ErrorMessage : public Message {
   public:
    ErrorMessage();
    std::string toString() const override;
};