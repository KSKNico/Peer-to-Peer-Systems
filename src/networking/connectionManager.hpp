#pragma once

#include <unordered_map>
#include <vector>

#include "../hash.hpp"
#include "acceptor.hpp"
#include "connection.hpp"
#include "connector.hpp"

class ConnectionManager {
   public:
    ConnectionManager(unsigned int port);
    void acceptAllConnections();

   private:
    ConnectionsMap connections;
    Acceptor acceptor;
    Connector connector;
};