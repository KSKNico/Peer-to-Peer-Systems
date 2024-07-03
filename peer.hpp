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

class Peer {
    public:
    Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses);
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
    ConnectionHandlerMap connections;

    // mutex for the connections unordered_map
    std::mutex connectionsMutex;

    Hash getHash() const;
    void run();

    private:
    std::unordered_map<unsigned long long, std::vector<unsigned long long>> prime_intervals;
    void processMessage(Message message, std::pair<const Hash, MyConnectionHandler *> connection);
    void process_put_message(Message message);
    void process_get_message(Message message, std::pair<const Hash, MyConnectionHandler*> connection);
    Hash id;
    Poco::Thread thread;
};
