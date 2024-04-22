#pragma once

#include <vector>
#include "connection.hpp"
#include "message.hpp"

class Sender{
    private:
    const std::vector<Connection>& connections;

    public:
    Sender() = delete;
    Sender(const std::vector<Connection>& Connections);

    /* Sends a message to all connections */
    void sendMessages(std::vector<Message> messages) const;
};