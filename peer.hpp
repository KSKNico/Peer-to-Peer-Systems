#pragma once
#include <string>
#include <vector>
#include <thread>
#include "listener.hpp"
#include "connector.hpp"
#include "connection.hpp"
#include "message.hpp"
#include <mutex>
#include <cstddef>
#include <array>

class Peer
{
private:
    std::mutex connectionMutex;
    Listener listener;
    Connector connector;
    std::vector<Connection> connections;

    std::queue<Message> receivedMessages;
    std::queue<Message> toSendMessages;
    
public:
    Peer() = delete;
    Peer(Peer& peer) = delete;
    Peer(Peer&& peer) = delete;
    Peer(std::vector<std::string> ipAddresses, int port);

    /* Converts an IPv4 address in the format x.x.x.x and a port to a socket address struct */
    static sockaddr_in convertToAddress(std::string ipAddress, int port);

    /* Add message to send */
    void addMessageToSend(Message message);

    /* Get all received messages */
    std::vector<Message> getReceivedMessages();

    /* */

    /* updates all connections events by polling the corresponding sockets */
    void pollConnections();

    /* reads all messages from the connections */
    void readMessages();

    /* write Messages */
    void writeMessages();
};
