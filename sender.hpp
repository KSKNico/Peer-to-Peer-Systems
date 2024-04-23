#pragma once

#include <vector>
#include <mutex>
#include "connection.hpp"
#include "message.hpp"

class Sender{
    private:
    const std::vector<Connection>& connections;
    std::mutex& connectionMutex;

    public:
    Sender() = delete;
    Sender(const std::vector<Connection>& Connections, std::mutex& connectionMutex);

    /* Sends a message to all connections */
    bool sendMessage(Message message) const;
};