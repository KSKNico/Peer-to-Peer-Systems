#pragma once 

#include "Poco/Net/SocketAcceptor.h"

#include "myConnectionHandler.hpp"

class MySocketAcceptor : public Poco::Net::SocketAcceptor<MyConnectionHandler> {
public:
    MySocketAcceptor(Poco::Net::ServerSocket& socket);

protected:
    MyConnectionHandler* createServiceHandler(const Poco::Net::StreamSocket& socket);
};