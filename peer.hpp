#pragma once
#include <string>
#include <vector>
#include <thread>
#include "listener.hpp"
#include "connector.hpp"
#include "connection.hpp"
#include "message.hpp"
#include <cstddef>
#include <array>

class Peer
{
private:
    Listener listener;
    Connector connector;
    std::vector<Connection> connections;
    
public:
    Peer() = delete;
    Peer(Peer& peer) = delete;
    Peer(Peer&& peer) = delete;
    Peer(uint16_t listeningPort, std::vector<sockaddr_in> addresses);
    Peer(uint16_t listeningPort);

    /* Converts an IPv4 address with a port in the format x.x.x.x:yyyy to a socket address struct */
    static sockaddr_in convertToAddress(std::string addressString);

    /* Reads one message from the connection */
    std::optional<Message> getMessage(Connection& connection);

    /* Reads all messages from the connections */
    std::vector<Message> getMessages();

    /* write Message */
    bool sendMessage(Message message);
    bool sendMessage(std::string messageString, sockaddr_in address);

    /* Accepts all incoming connections and adds them to the connections vector */
    void acceptConnections();

    /* Connects to address */
    void connectTo(sockaddr_in address);
};
