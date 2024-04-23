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
#include <mutex>

class Peer
{
private:
    std::mutex connectionMutex;
    Listener listener;
    Connector connector;
    Receiver receiver;
    Sender sender;
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

    /* Starts the listener in a loop */
    void startListener();

    /* Start the receiver in a loop */
    void startReceiver();

    /* Start the sender in a loop */
    void startSender();
};
