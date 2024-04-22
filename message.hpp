#pragma once
#include <arpa/inet.h>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>
#include <queue>

class Message {
private:
uint32_t length;
sockaddr_in from;
sockaddr_in to;
uint32_t id;
std::string payload;



public:
Message(sockaddr_in from, sockaddr_in to, unsigned int id, std::string payload);

/* Serialize the message to a byte sequence */
std::vector<std::byte> serialize();

/* Deserialize a byte sequence to a message */
static Message deserialize(std::vector<std::byte> messageData);

/* Serialize IP address and port */
static std::vector<std::byte> serializeAddress(sockaddr_in address);

/* Deserialize IP address and port */
static sockaddr_in deserializeAddress(std::vector<std::byte> addressData);

/* Get the receiver */
sockaddr_in getReceiver() const;
};