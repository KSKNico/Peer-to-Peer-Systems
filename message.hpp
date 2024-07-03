#pragma once
#include <string>
#include <array>
#include <cstdint>

#include "Poco/Buffer.h"

// a message is ascii encoded and is always 1024 bytes long
class Message {
public:
    /*
     * structure of GET: "<type>, <IP address from sender>, <first number of requested interval>"
     * example: if peer 127.0.0.1:5001 wants to get primes of the interval [1, 1000] from peer 127.0.0.1:5002 it sends:
     * "GET,127.0.0.1:5001,1" so "," is the delimiter
     *
     * structure of PUT: "<type>, <IP address from sender>, <first number of interval>[, value1, value2, ...]"
     * example: if peer 127.0.0.1:5001 wants to put the primes 2 and 3 in the interval [1, 1000] for which
     * peer 127.0.0.1:5002 is responsible it sends:
     * "PUT,127.0.0.1:5001,1,2,3"
     * if this PUT is the answer of a GET message and the peer has yet no results for this interval it sends:
     * "PUT,127.0.0.1:5001,1"
     */
    enum class MessageType {
        GET,
        PUT,
        JOIN,
    };
    struct get_message{
        std::string IP_address;
        unsigned long long start_of_interval;
    };
    struct put_message{
        std::string IP_address;
        unsigned long long start_of_interval;
        std::vector<unsigned long long> primes;
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
    static Poco::Buffer<char> toBuffer(const Message &message);
    
    get_message decode_get_message();
    put_message decode_put_message();
};