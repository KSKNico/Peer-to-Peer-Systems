#pragma once

#include <vector>
#include <map>
#include <stdexcept>
#include "connection.hpp"
#include "message.hpp"

class Receiver {
    private:
    const std::vector<Connection> &connections;

    public:
    Receiver() = delete;
    Receiver(const std::vector<Connection>& Connections);

    /* Gets the next message from all connections */
    std::vector<Message> receiveMessages();
};