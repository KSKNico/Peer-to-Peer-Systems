#include "iointerface.hpp"

Message IOInterface::dequeueIncomingMessage() {
    std::lock_guard<std::mutex> lock(incomingMutex);
    if (incoming.empty()) {
        return Message({'\0'});
    }
    auto message = incoming.front();
    incoming.pop();
    return message;
}

void IOInterface::queueOutgoingMessage(Message message) {
    std::lock_guard<std::mutex> lock(outgoingMutex);
    outgoing.push(message);
}

void IOInterface::queueIncomingMessage(Message message) {
    std::lock_guard<std::mutex> lock(incomingMutex);
    incoming.push(message);
}

Message IOInterface::dequeueOutgoingMessage() {
    std::lock_guard<std::mutex> lock(outgoingMutex);
    if (outgoing.empty()) {
        return Message::MessageData({'\0'});
    }
    auto message = outgoing.front();
    outgoing.pop();
    return message;
}

std::size_t IOInterface::getIncomingMessageCount() {
    std::lock_guard<std::mutex> lock(incomingMutex);
    return incoming.size();
}

std::size_t IOInterface::getOutgoingMessageCount() {
    std::lock_guard<std::mutex> lock(outgoingMutex);
    return outgoing.size();
}