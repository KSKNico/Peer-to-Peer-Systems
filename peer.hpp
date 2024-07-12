#pragma once

#include <vector>
#include <unordered_map>

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketConnector.h"
#include "Poco/Timespan.h"
#include "Poco/Net/NetException.h"

#include "hash.hpp"
#include "myConnectionHandler.hpp"
#include "mySocketConnector.hpp"
#include "mySocketAcceptor.hpp"
#include "sectorHandling/sectorHandler.hpp"

class Peer {
    public:
    // Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses);
    Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress);
    Peer(Poco::Net::SocketAddress ownAddress);
    Poco::Net::ServerSocket serverSocket;
    // FingerTable fingerTable;
    Poco::Net::SocketAddress address;

    Poco::Net::SocketReactor reactor;

    MySocketAcceptor acceptor;
    std::unordered_map<Hash, std::unique_ptr<MySocketConnector>, Hash::Hasher> connectors;

    // needs to be locked with the connectionsMutex
    using ConnectionHandlerMap = std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>;

    // THIS HAS TO BE LOCKED WHENEVER ACCESSED WITH THE CONNECTIONS MUTEX!!!
    ConnectionHandlerMap connections;

    // mutex for the connections unordered_map
    std::mutex connectionsMutex;

    unsigned long long new_interval_start = 0;

    Hash getHash() const;
    void run();

    void printConnections();

    std::unordered_map<unsigned long long, std::vector<unsigned long long>> prime_intervals;
    void processMessage(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    void process_put_message(Message message);
    void process_get_message(Message message, std::pair<const Hash, MyConnectionHandler*> connection);
    void process_getack_message(Message message);
    void process_join_message(Message message, std::pair<const Hash, MyConnectionHandler*> connection);
    void process_joinack_message(Message message);
    void process_succ_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    void process_succack_message(Message message);
    void process_pred_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    void process_predack_message(Message message);
    void process_fing_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    void process_fingack_message(Message message);
    void process_find_interval_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    void process_find_interval_ack_message(Message message);
    void initFingerTable(MyConnectionHandler * successorConnection);

    std::string findClosestPeer(Hash& position);

    sectorHandler sectorHandler;

    std::unordered_map<Hash, Poco::Net::SocketAddress, Hash::Hasher> fingerTable;
    Poco::Net::SocketAddress successor = address;   // initially you are your successor
    Poco::Net::SocketAddress predecessor = address; // ~ predecessor

    std::unordered_map<Hash, std::vector<Message>, Hash::Hasher> outgoingMessages;

    Hash id;
    Poco::Thread thread;

    void stabilize();
};
