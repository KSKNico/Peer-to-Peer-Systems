#pragma once

#include <unordered_map>
#include <vector>

#include "acceptor.hpp"
#include "connection.hpp"
#include "connector.hpp"

class ConnectionManager {
   public:
    ConnectionManager(unsigned int port);

   private:
    std::unordered_map<Poco::Net::SocketAddress, Connection> connetions;
    Acceptor acceptor;
    Connector connector;
};