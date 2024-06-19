#pragma once

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketConnector.h"
#include "fingertable.hpp"
#include "Poco/Timespan.h"

class Peer {
    public:
    Peer(int port, std::vector<Poco::Net::SocketAddress> knownHosts);
    Poco::Net::ServerSocket serverSocket;
    // FingerTable fingerTable;
    Poco::Net::SocketAddress address;

    Hash getHash() const;

    private:
    Poco::Net::SocketAddress detectOwnIPAddress(Poco::Net::SocketAddress const &knownHost) const;
    Hash id;
};
