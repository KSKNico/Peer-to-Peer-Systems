#pragma once

#include <unordered_map>
#include <vector>

#include "Poco/Net/NetException.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketConnector.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Timespan.h"
#include "fingerTable.hpp"
#include "networking/connectionManager.hpp"
#include "taskManager.hpp"

class Peer {
   public:
    // Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses);
    Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress);
    Peer(Poco::Net::SocketAddress ownAddress);

    void update();

   private:
    const Poco::Net::SocketAddress ownAddress;
    ConnectionManager connectionManager;
    FingerTable fingerTable;
    TaskManager taskManager;
};
