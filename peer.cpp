#include "peer.hpp"

Peer::Peer(std::vector<std::string> ipAddresses, int port)
{
    for (std::string ipAddress : ipAddresses)
    {
        struct sockaddr_in address = convertToAddress(ipAddress, port);
        Connection connection = connector.connectTo(address);
        connections.push_back(connection);
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

void Peer::readMessages() {
    for (auto& connection : connections)
    {
        if (connection.getReturnEvents() & POLLIN)
        {
            // TODO
        }
    }
}

void Peer::writeMessages() {
}

sockaddr_in Peer::convertToAddress(std::string ipAddress, int port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr);
    return address;
}

