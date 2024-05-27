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

// TODO: this function is a bit of a mess, because the polling should be done on the connection object 
// but poll allows for multiple file descriptors to be polled at once, reducing the amount of system calls (?)
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

void Peer::sendMessage(std::string message_string, sockaddr_in address)  {
    // construct a message first
    Message message = Message(listener.address, address, 0, message_string);
    sendMessage(message);
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