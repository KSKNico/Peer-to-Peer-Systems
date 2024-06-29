#pragma once

#include "message.hpp"

#include <queue>
#include <mutex>

// This class is used to provide a thread-safe interface for the socket to communicate with the app
class IOInterface {
public:
    // can be used by the app to get the next incoming message,
    // if there is no message, it will return an empty message
    Message dequeueIncomingMessage();

    // can be used by the app to queue an outgoing message
    void queueOutgoingMessage(Message message);

    // is intended for the use of the socket
    void queueIncomingMessage(Message message);

    // is intended for the use of the socket
    Message dequeueOutgoingMessage();

    std::size_t getIncomingMessageCount();

    std::size_t getOutgoingMessageCount();

private:
    // incoming messages
    std::queue<Message> incoming;

    // outgoing messages
    std::queue<Message> outgoing;

    std::mutex incomingMutex;
    std::mutex outgoingMutex;
};