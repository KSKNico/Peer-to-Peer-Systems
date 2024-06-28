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
    std::vector<std::unique_ptr<MySocketConnector>> connectors;

    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher> connections;

    Hash getHash() const;

    private:
    Hash id;
    Poco::Thread thread;
};
