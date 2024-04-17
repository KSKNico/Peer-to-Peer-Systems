#include "peer.hpp"

Peer::Peer(std::vector<std::string> ipAddresses, int port)
{
    // start thread for listener
    std::thread listenerThread(&Peer::startListener, &(*this));
    for (std::string ipAddress : ipAddresses)
    {
        struct sockaddr_in address = convertToAddress(ipAddress, port);
        Connection connection = connector.connectTo(address);
        connections.push_back(connection);
    }
}

sockaddr_in Peer::convertToAddress(std::string ipAddress, int port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &address.sin_addr);
    return address;
}

void Peer::startListener() {
    while (true)
    {
        Connection connection = listener.acceptConnection();
        std::cout << "Connection accepted" << std::endl;
        connections.push_back(connection);
    }
}

void Peer::sendAll(Message message) {
    for (Connection connection : connections)
    {
        auto data = message.serialize();
        connection.sendData(data);
    }
}

std::vector<Message> Peer::receiveAll() {
    std::vector<Message> messages;
    for (Connection connection : this->connections)
    {
        std::string data = connection.receiveData();
        Message message = Message::deserialize(data);
        messages.push_back(message);
    }
    return messages;
}

