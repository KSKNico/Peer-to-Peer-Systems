#pragma once 

#include "Poco/Net/SocketAcceptor.h"

#include "myConnectionHandler.hpp"

class MySocketAcceptor : public Poco::Net::SocketAcceptor<MyConnectionHandler> {
public:
    MySocketAcceptor(Poco::Net::ServerSocket& socket, Poco::Net::SocketReactor& reactor);

protected:
    MyConnectionHandler* createServiceHandler(Poco::Net::StreamSocket& socket);
};