#include "sender.hpp"

Sender::Sender(const std::vector<Connection>& Connections) : connections(Connections) {}

void Sender::sendMessages(std::vector<Message> messages) const {
    for (auto& connection : connections) {
        for (auto& message : messages) {
            auto messageData = message.serialize();
            connection.sendData(messageData);
        }
    }
}