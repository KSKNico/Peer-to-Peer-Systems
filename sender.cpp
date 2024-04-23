#include "sender.hpp"

bool operator==(const sockaddr_in& lhs, const sockaddr_in& rhs) {
    return lhs.sin_addr.s_addr == rhs.sin_addr.s_addr && lhs.sin_port == rhs.sin_port;
}

bool operator!=(const sockaddr_in& lhs, const sockaddr_in& rhs) {
    return !(lhs == rhs);
}

Sender::Sender(const std::vector<Connection>& Connections, std::mutex& connectionMutex) :
connections(Connections), connectionMutex(connectionMutex) {}

bool Sender::sendMessage(Message message) const {
    std::lock_guard<std::mutex> lock(connectionMutex);
    for (auto& connection : connections) {
        // check if message corresponds to the connection
        if (message.getReceiver() != connection.getAddress()) {
            continue;
        }
        auto messageData = message.serialize();
        connection.sendData(messageData);
        return true;
    }
}