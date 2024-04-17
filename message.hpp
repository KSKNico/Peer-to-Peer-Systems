#pragma once
#include <arpa/inet.h>
#include <string>
#include <vector>

class Message {
private:
sockaddr_in from;
sockaddr_in to;
unsigned int id;
std::string message;


public:
Message(sockaddr_in from, sockaddr_in to, unsigned int id, std::string message);

/* Serialize the message to a string */
std::string serialize();

/* Deserialize a string to a message */
static Message deserialize(std::string message);
};