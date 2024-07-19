#pragma once

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "connection.hpp"

class Acceptor {
   public:
    Acceptor(unsigned int port);

    std::unique_ptr<Connection> accept();

    bool hasConnection();

   private:
    Poco::Net::ServerSocket serverSocket;
};