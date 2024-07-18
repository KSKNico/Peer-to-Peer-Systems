#pragma once

#include <unordered_map>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"

class Connector {
   public:
    void connect(Poco::Net::SocketAddress address);

   private:
    std::unordered_map<Poco::Net::SocketAddress, Poco::Net::StreamSocket> outboundConnections;
};