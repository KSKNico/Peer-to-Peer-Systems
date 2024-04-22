#include "receiver.hpp"

Receiver::Receiver(const std::vector<Connection>& Connections) : connections(Connections) {}

std::vector<Message> Receiver::receiveMessages() {
    // the idea is to get exactly one message from each connection
    // if a connection has no data to receive, it will return no message
    std::vector<Message> messages;
    for (auto& connection : connections) {
        std::vector<std::byte> lengthData = connection.receiveData(4);
        if (lengthData.empty() ) {
            continue;
        }

        if (lengthData.size() < 4) {
            throw std::runtime_error("Invalid message size");
        }

        auto size = *reinterpret_cast<uint32_t*>(lengthData.data());

        std::vector<std::byte> messageData = connection.receiveData(size-4);
        if (messageData.size() < size-4) {
            throw std::runtime_error("Invalid message size");
        }

        // join length and message data
        lengthData.insert(lengthData.end(), messageData.begin(), messageData.end());
        messages.push_back(Message::deserialize(lengthData));
    }
    return messages;
}

