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

void Peer::pollConnections() {
    std::vector<pollfd> pollStructs;
    for (auto& connection : connections)
    {
        pollfd pollStruct = pollfd();
        pollStruct.fd = connection.getSocket();
        pollStruct.events = POLLIN;
        pollStruct.revents = 0;
        pollStructs.push_back(pollStruct);
    }

    poll(pollStructs.data(), pollStructs.size(), -1);

    // update the connections with the new return events
    for (size_t i = 0; i < connections.size(); i++)
    {
        connections[i].updateReturnEvents(pollStructs[i].revents);
    }
}

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
    if (!(connection.getReturnEvents() & POLLIN))
    {
        return message;
    }
    
    message = connection.readMessage();

    return message;
}   

void Peer::sendMessage(Message message) {
    for (auto& connection : connections)
    {
        if (connection.getAddress() == message.getReceiver()) {
            connection.writeMessage(message);
        }   
    }
}

