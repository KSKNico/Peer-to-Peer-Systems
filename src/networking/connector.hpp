#pragma once

#include <unordered_map>

#include "../hash.hpp"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "connection.hpp"

class Connector {
   public:
    std::unique_ptr<Connection> connect(const Poco::Net::SocketAddress &address, bool blocking = true);
};