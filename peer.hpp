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

    /* Updates all connections events by polling the corresponding sockets */
    void pollConnections();

    /* Reads one message from the connection */
    std::optional<Message> getMessage(Connection& connection);

    /* Reads all messages from the connections */
    std::vector<Message> getMessages();

    /* write Message */
    void sendMessage(Message message);
};
