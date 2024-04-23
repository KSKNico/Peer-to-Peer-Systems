#include "peer.hpp"

Peer::Peer(std::vector<std::string> ipAddresses, int port) : connectionMutex(), receiver(connections, connectionMutex), sender(connections, connectionMutex)
{
    // start thread for listener
    for (std::string ipAddress : ipAddresses)
    {
        struct sockaddr_in address = convertToAddress(ipAddress, port);
        Connection connection = connector.connectTo(address);
        connections.push_back(connection);
    }
    // TODO: these require locking!
    std::thread listenerThread(&Peer::startListener, &(*this));
    std::thread receiverThread(&Peer::startReceiver, &(*this));
    std::thread senderThread(&Peer::startSender, &(*this));
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
        std::unique_lock<std::mutex> lock(connectionMutex);
        connections.push_back(connection);
        lock.unlock();
    }
}

void Peer::startReceiver() {
    std::vector<Message> messages;
    while (true)
    {
        messages = receiver.receiveMessages();
        for (Message message : messages)
        {
            receivedMessages.push(message);
        }
    }
}

void Peer::startSender() {
    while (true)
    {
        if (!toSendMessages.empty())
        {
            // TODO: send messages
            auto message = toSendMessages.front();
            sender.sendMessage(message);
            toSendMessages.pop();
        }
    }
}

