#pragma once

#include "Poco/Net/SocketConnector.h"

#include "myConnectionHandler.hpp"
#include "mySocketConnector.hpp"
#include <unordered_map>
#include "hash.hpp"

class MySocketConnector : public Poco::Net::SocketConnector<MyConnectionHandler> {
public:
    MySocketConnector(const Poco::Net::SocketAddress& address, 
    Poco::Net::SocketReactor& reactor,
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections,
    std::mutex& connectionsMutex
    );

    bool isFinished() const;

protected:
    MyConnectionHandler* createServiceHandler();

private:
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections;
    std::mutex& connectionsMutex;
    bool finished;
    Poco::Net::SocketAddress remoteAddress;
};