#pragma once

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketConnector.h"
#include "Poco/Timespan.h"
#include "Poco/Net/NetException.h"

#include "fingertable.hpp"

class Peer {
    public:
    Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> knownHosts);
    Poco::Net::ServerSocket serverSocket;
    // FingerTable fingerTable;
    Poco::Net::SocketAddress address;

    Hash getHash() const;

    private:
    Hash id;
};
