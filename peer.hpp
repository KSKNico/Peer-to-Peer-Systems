#pragma once

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketConnector.h"
#include "Poco/Timespan.h"
#include "Poco/Net/NetException.h"

#include "fingertable.hpp"
#include "myConnectionHandler.hpp"
#include "mySocketConnector.hpp"
#include "mySocketAcceptor.hpp"

class Peer {
    public:
    Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress);
    Peer(Poco::Net::SocketAddress ownAddress);
    Poco::Net::ServerSocket serverSocket;
    // FingerTable fingerTable;
    Poco::Net::SocketAddress address;

    Poco::Net::SocketReactor reactor;

    Poco::Thread thread;

    Hash getHash() const;

    private:
    Hash id;
};
