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
    FINDR,
    SSUC,
    GSUC,
    GSUCR,
    SPRE,
    GPRE,
    GPRER,
    INCOMPLETE,
    ERRORED,
    UNKNOWN,
};

class Message {
   public:
    virtual ~Message() {};

    virtual std::string toString() const = 0;
    // static Message fromString(const std::string &str);
    // static std::string extractHead(const Poco::FIFOBuffer &buffer);
    static std::string extractHead(const std::string &str);

    static MessageType getMessageTypeFromString(const std::string &str);

    static std::string messageTypeToString(const MessageType type);

    virtual bool isComplete() const final;
    virtual bool isErrored() const final;
    virtual MessageType getType() const final;

   protected:
    MessageType type;
    Poco::Net::SocketAddress from;
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
    JoinMessage();
    std::string toString() const override;
    static JoinMessage fromString(const std::string &str);

    static constexpr std::string head = "JOIN";
};

// used to find a peer that is responsible for a certain hash
class FindMessage : public Message {
   public:
    FindMessage(const Hash &hash, const Poco::Net::SocketAddress &origin);
    std::string toString() const override;
    static FindMessage fromString(const std::string &);

    static constexpr std::string head = "FIND";

    // target that needs to be found in the network
    const Hash target;

    // what peer has initially issued this find task
    const Poco::Net::SocketAddress origin;
};

// contains the hash that has been found
// this message is to be sent back by the correct peer 
// to the peer that started the find task
class FindResponseMessage : public Message {
   public:
    FindResponseMessage(const Hash &target);
    std::string toString() const override;
    static FindResponseMessage fromString(const std::string &);

    static constexpr std::string head = "FINDR";

    // target that has been found
    const Hash target;
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

class GetSuccessorMessage : public Message {
   public:
    GetSuccessorMessage() = default;
    std::string toString() const override;
    static GetSuccessorMessage fromString(const std::string &str);

    static constexpr std::string head = "GSUC";
};

class GetPredecessorMessage : public Message {
   public:
    GetPredecessorMessage() = default;
    std::string toString() const override;
    static GetPredecessorMessage fromString(const std::string &str);

    static constexpr std::string head = "GPRE";
};

class GetSuccessorResponseMessage : public Message {
   public:
    GetSuccessorResponseMessage(const Poco::Net::SocketAddress &successor);
    std::string toString() const override;
    static GetSuccessorResponseMessage fromString(const std::string &str);

    static constexpr std::string head = "GSUCR";

   private:
    const Poco::Net::SocketAddress successor;
};

class GetPredecessorResponseMessage : public Message {
   public:
    GetPredecessorResponseMessage(const Poco::Net::SocketAddress &predecessor);
    std::string toString() const override;
    static GetPredecessorResponseMessage fromString(const std::string &str);

    static constexpr std::string head = "GPRER";

    Poco::Net::SocketAddress getPredecessor() const;

   private:
    const Poco::Net::SocketAddress predecessor;
};

// the receveiving peer should simply set its own successor to the address in the message
class SetSuccessorMessage : public Message {
   public:
    SetSuccessorMessage(const Poco::Net::SocketAddress &successor);
    std::string toString() const override;
    static SetSuccessorMessage fromString(const std::string &str);

    static constexpr std::string head = "SSUC";

   private:
    const Poco::Net::SocketAddress successor;
};

class SetPredecessorMessage : public Message {
   public:
    SetPredecessorMessage(const Poco::Net::SocketAddress &newPredecessor);
    std::string toString() const override;
    static SetPredecessorMessage fromString(const std::string &str);

    Poco::Net::SocketAddress getNewPredecessor() const;

    static constexpr std::string head = "SPRE";

   private:
    const Poco::Net::SocketAddress newPredecessor;
};