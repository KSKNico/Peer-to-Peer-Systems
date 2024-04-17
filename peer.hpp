#pragma once
#include <string>
#include <vector>
#include <thread>
#include "listener.hpp"
#include "connector.hpp"
#include "connection.hpp"
#include "message.hpp"

struct OtherPeer {
    sockaddr_in address;
    int connectionCount;
    int timestamp;
};

class Peer
{
private:
    Listener listener;
    Connector connector;
    std::vector<Connection> connections;
    std::vector<OtherPeer> otherPeers; 
    
public:
    Peer(std::vector<std::string> ipAddresses, int port);

    /* Converts an IPv4 address in the format x.x.x.x and a port to a socket address struct */
    static sockaddr_in convertToAddress(std::string ipAddress, int port);
    void startListener();

    /* Sends a message to all active connections */
    void sendAll(Message message);

    /* Receives messages from all active connections */
    std::vector<Message> receiveAll();
};
