#pragma once
#include <string>
#include <vector>
#include <thread>
#include "listener.hpp"
#include "connector.hpp"
#include "connection.hpp"
#include "message.hpp"
#include "sender.hpp"
#include "receiver.hpp"

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
    Receiver receiver;
    Sender sender;
    std::vector<Connection> connections;
    std::vector<OtherPeer> otherPeers;

    std::queue<Message> receivedMessages;
    std::queue<Message> toSendMessages;
    
public:
    Peer() = delete;
    Peer(Peer& peer) = delete;
    Peer(Peer&& peer) = delete;
    Peer(std::vector<std::string> ipAddresses, int port);

    /* Converts an IPv4 address in the format x.x.x.x and a port to a socket address struct */
    static sockaddr_in convertToAddress(std::string ipAddress, int port);

    /* Starts the listener in a loop */
    void startListener();

    /* Start the receiver in a loop */
    void startReceiver();

    /* Start the sender in a loop */
    void startSender();
};
