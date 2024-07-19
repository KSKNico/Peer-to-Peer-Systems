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
    UNKNOWN,
};

class Message {
   public:
    Message();

    virtual std::string toString() const;
    static Message fromString(const std::string &str);
    static std::string extractHead(const Poco::FIFOBuffer &buffer);

    static MessageType getMessageType(const std::string &str);
    static std::string messageTypeToString(const MessageType type);

   protected:
    MessageType type;
};

// used to exchange the id of a peer
class IDMessage : public Message {
   public:
    IDMessage(Poco::Net::SocketAddress);
    std::string toString() const override;
    static IDMessage fromString(const std::string &str);
    static constexpr std::string head = "ID";

   private:
    Poco::Net::SocketAddress ownAddress;
};

class JoinMessage : public Message {
   public:
    JoinMessage(Hash);
    std::string toString() const override;
    static Message fromString(const std::string &str);
    static constexpr std::string head = "JOIN";

   private:
    Hash id;
};