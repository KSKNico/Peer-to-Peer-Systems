#include "iointerface.hpp"

Message IOInterface::dequeueIncomingMessage() {
    std::unique_lock<std::mutex> lock(incomingMutex);
    if (incoming.empty()) {
        return Message("");
    }
    auto message = incoming.front();
    incoming.pop();
    return message;
}

void IOInterface::queueOutgoingMessage(Message message) {
    std::unique_lock<std::mutex> lock(outgoingMutex);
    outgoing.push(message);
}

void IOInterface::queueIncomingMessage(Message message) {
    std::unique_lock<std::mutex> lock(incomingMutex);
    incoming.push(message);
}

Message IOInterface::dequeueOutgoingMessage() {
    std::unique_lock<std::mutex> lock(outgoingMutex);
    if (outgoing.empty()) {
        return Message("");
    }
    auto message = outgoing.front();
    outgoing.pop();
    return message;
}

std::size_t IOInterface::getIncomingMessageCount() {
    std::unique_lock<std::mutex> lock(incomingMutex);
    return incoming.size();
}

std::size_t IOInterface::getOutgoingMessageCount() {
    std::unique_lock<std::mutex> lock(outgoingMutex);
    return outgoing.size();
}