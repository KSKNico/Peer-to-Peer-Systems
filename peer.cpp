#include "peer.hpp"

bool operator==(const sockaddr_in& lhs, const sockaddr_in& rhs)
{
    return lhs.sin_addr.s_addr == rhs.sin_addr.s_addr && lhs.sin_port == rhs.sin_port;
}

Peer::Peer(uint16_t listeningPort, std::vector<sockaddr_in> addresses) : listener(listeningPort), connector() {
    for (auto& address : addresses)
    {
        connections.push_back(connector.connectTo(address));
    }
}

Peer::Peer(uint16_t listeningPort) : listener(listeningPort), connector() {}

std::vector<Message> Peer::getMessages() {
    auto messages = std::vector<Message>();
    for (auto& connection : connections)
    {
        auto message = getMessage(connection);

        if (!message.has_value())
        {
            continue;
        }
        messages.push_back(message.value());
    }
    return messages;
}

std::optional<Message> Peer::getMessage(Connection& connection) {
    auto message = std::optional<Message>();
    if (connection.hasData())
    {
        message = connection.readMessage();
    }

    return message;
}   

bool Peer::sendMessage(Message message) {
    for (auto& connection : connections)
    {
        if (!(connection.getAddress() == message.getReceiver())) {
            continue;
        }    

        if (connection.isValid() && connection.isWritable()) {
            connection.sendData(message.serialize());
            return true;
        }

        return false;
    }
}

bool Peer::sendMessage(std::string messageString, sockaddr_in address)  {
    // construct a message first
    Message message = Message(listener.address, address, 0, messageString);
    return sendMessage(message);
}

void Peer::acceptConnections() {
    while (listener.pollForConnection())
    {
        connections.push_back(listener.acceptConnection());
    }
}

void Peer::connectTo(sockaddr_in address) {
    connections.push_back(connector.connectTo(address));
}