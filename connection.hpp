#pragma once
#include "arpa/inet.h"
#include "unistd.h"
#include "sys/socket.h"
#include "sys/poll.h"
#include <vector>
#include <array>
#include <cstddef>
#include "message.hpp"
#include <optional>

class Connection
{
    private:
    static constexpr int bufferSize = 1024;
    int socket;
    short int events;
    short int revents;
    sockaddr_in address;
    std::array<std::byte, bufferSize> buffer;
    int offset;
    bool firstPart;
    uint32_t length;
    uint32_t remaining;

    public:
    Connection(int socket, struct sockaddr_in address);
    ~Connection();

    /* Sends variable length data over the network */
    void sendData(std::vector<std::byte> data) const;

    /* Get return events */
    short getReturnEvents() const;

    /* Update return events */
    void updateReturnEvents(short revents);

    /* Returns at most (bufferSize - offset) bytes from this connection */
    int receiveData();

    /* Gets a single message if at all. A message can't be longer than bufferSize */
    std::optional<Message> getMessage();

    /* Get the connection address */
    sockaddr_in getAddress() const;

    /* Get the socket id */
    int getSocket() const;
};