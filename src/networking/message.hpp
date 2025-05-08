#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

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
    STORE,
    QUERY,
    QRESP,
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
    static std::unique_ptr<Message> fromString(const std::string &str);

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

// the query message is used to ask results
// the response to a query message is always a QueryResponseMessage
// if the query message does not contain a query it request the highest available results on the peer
// if the query message contains a query it requests the results that the peer stores with that intervals
// if there are no results or the requested query is not found on the peer, it sends an empty QueryResponseMessage
class QueryMessage : public Message {
    public:
     QueryMessage(resultType query);
     QueryMessage();
     std::string toString() const override;
     static QueryMessage fromString(const std::string &str);
    
     static constexpr std::string head = "QUERY";
    
     std::optional<resultType> getQuery() const;
    
    private:
     const std::optional<resultType> query;
};

class StoreMessage : public Message {
    public:
    StoreMessage(uint64_t query, const std::vector<uint64_t> &results);
    std::string toString() const override;
    static StoreMessage fromString(const std::string &str);

    static constexpr std::string head = "STORE";

    uint64_t getQuery() const;
    std::vector<uint64_t> getResults() const;

    private:
    const std::uint64_t query;
    const std::vector<uint64_t> results;
};

// the query response message can be empty if the query was not found
class QueryResponseMessage : public Message {
    public:
    QueryResponseMessage(resultType query, const resultContainer &results);
    QueryResponseMessage();
    std::string toString() const override;
    static QueryResponseMessage fromString(const std::string &str);

    static constexpr std::string head = "QRESP";

    std::optional<resultType> getQuery() const;
    std::optional<resultContainer> getResults() const;

    private:
    const std::optional<resultType> query;
    const std::optional<resultContainer> results;
};