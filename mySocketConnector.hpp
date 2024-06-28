#pragma once

#include "Poco/Net/SocketConnector.h"

#include "myConnectionHandler.hpp"

class MySocketConnector : public Poco::Net::SocketConnector<MyConnectionHandler> {
public:
    MySocketConnector(const Poco::Net::SocketAddress& address, Poco::Net::SocketReactor& reactor);

protected:
    MyConnectionHandler* createServiceHandler();

private:
    
};