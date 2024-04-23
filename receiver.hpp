#pragma once

#include <vector>
#include <map>
#include <stdexcept>
#include <mutex>
#include "connection.hpp"
#include "message.hpp"

class Receiver {
    private:
    const std::vector<Connection> &connections;
    std::mutex &connectionMutex;

    public:
    Receiver() = delete;
    Receiver(const std::vector<Connection>& Connections, std::mutex& connectionMutex);

    /* Gets the next message from all connections */
    std::vector<Message> receiveMessages();
};