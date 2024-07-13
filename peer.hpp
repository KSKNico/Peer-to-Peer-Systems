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
#include "resultHandler.hpp"
#include "timing.hpp"

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

    Timing timing;

    Hash getHash() const;
    void run();

    //TODO add some descriptions
    void printConnections();
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
    
    //send stabilize message to pred and succ
    void stabilize();
    //return STABACK Message to sender, send out a PRED and SUCC message
    void process_stabilize_message(Message message);
    //updates current pred or succ if necessary
    void process_stabilizeack_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    //starts iterating through the ring and fills fingertable
    void findFingers();

    void doIntervalRoutine();
    void doStabilizeRoutine();
    void doFindFingersRoutine();

    std::string findClosestPeer(Hash& position);

    ResultHandler resultHandler;

    std::unordered_map<Hash, Poco::Net::SocketAddress, Hash::Hasher> fingerTable;
    Poco::Net::SocketAddress successor = address;   // initially you are your successor
    Poco::Net::SocketAddress predecessor = address; // ~ predecessor

    std::unordered_map<Hash, std::vector<Message>, Hash::Hasher> outgoingMessages;

    Hash id;
    Poco::Thread thread;

};
