#pragma once
#include <string>
#include <array>
#include <cstdint>
#include <vector>

#include "Poco/Buffer.h"
#include "Poco/FIFOBuffer.h"

// a message is ascii encoded and is always 1024 bytes long
class Message {
public:
    /*
     * structure of GET: "<type>, <IP address from sender>, <first number of requested interval>"
     * example: if peer 127.0.0.1:5001 wants to get primes of the interval [1, 1000] from peer 127.0.0.1:5002 it sends:
     * "GET,127.0.0.1:5001,1" so "," is the delimiter
     *
     * structure of GETACK : "<type>, <IP address from sender>, <IP address for routing>"
     *
     * structure of PUT: "<type>, <IP address from sender>, <first number of interval>[, value1, value2, ...]"
     * example: if peer 127.0.0.1:5001 wants to put the primes 2 and 3 in the interval [1, 1000] for which
     * peer 127.0.0.1:5002 is responsible it sends:
     * "PUT,127.0.0.1:5001,1,2,3"
     * if this PUT is the answer of a GET message and the peer has yet no results for this interval it sends:
     * "PUT,127.0.0.1:5001,1"
     *
     * structure of JOIN: "<type>, <IP address from sender>"
     *
     * structure of JOINACK: "<type>, <IP address from sender>, <IP of closest known peer before searcher in the circle>"
     *
     * structure of SUCC: "<type>, <IP address from sender>"
     *
     * structure of SUCCACK: "<type>, <IP address from sender>, <IP of closest known peer>"
     *
     * structure of PRED: "<type>, <IP address from sender>"
     *
     * structure of PREDACK: "<type>, <IP address from sender>, <IP of closest known peer>"
     *
     * closest IP in SUCCACK should be in the last step the successor of the requester
     *
     * structure of FING: "<type>, <IP address from sender>"
     *
     * structure of FINGACK: "<type>, <IP address from sender>, <IP address of successor of sender>"
     */
    enum class MessageType {
        GET,
        GETACK,
        PUT,
        JOIN,
        JOINACK,
        SUCC,
        SUCCACK,
        PRED,
        PREDACK,
        FING,
        FINGACK,
    };
    struct get_message{
        std::string IP_address;
        unsigned long long start_of_interval;
    };
    struct getack_message{
        std::string IP_address;
        unsigned long long start_of_interval;
        std::string RoutingIP;
    };
    struct put_message{
        std::string IP_address;
        unsigned long long start_of_interval;
        std::vector<unsigned long long> primes;
    };
    struct join_message{
        std::string IP_address;
    };
    struct joinack_message{
        std::string IP_address;
        std::string ClosestKnownIP;
    };
    struct succ_message{
        std::string IP_address;
    };
    struct succack_message{
        std::string IP_address;
        std::string ClosestKnownIP;
    };
    struct pred_message{
        std::string IP_address;
    };
    struct predack_message{
        std::string IP_address;
        std::string currentPred;
    };
    struct fing_message{
        std::string IP_address;
    };
    struct fingack_message{
        std::string IP_address;
        std::string SuccessorIP;
    };
    constexpr static int FIXED_MESSAGE_SIZE = 1024;

    // used inside of a message to separate parts
    constexpr static char PART_DELIMITER = ' ';

    using MessageData = std::array<char, FIXED_MESSAGE_SIZE>;

    Message(MessageData data);
    MessageType type;
    MessageData data;

    bool isEmpty() const;

    MessageData serialize();
    static Message deserialize(MessageData data);

    static Message fromBuffer(const Poco::Buffer<char> &buffer);
    static Message fromBuffer(Poco::BasicFIFOBuffer<char> &buffer);
    static Poco::Buffer<char> toBuffer(const Message &message);

    get_message decode_get_message();
    getack_message decode_getack_message();
    put_message decode_put_message();
    join_message decode_join_message();
    joinack_message decode_joinack_message();
    succ_message decode_succ_message();
    succack_message decode_succack_message();
    pred_message decode_pred_message();
    predack_message decode_predack_message();
    fing_message decode_fing_message();
    fingack_message decode_fingack_message();
};