#pragma once 

#include "Poco/Net/SocketAcceptor.h"

#include "myConnectionHandler.hpp"
#include <unordered_map>
#include "hash.hpp"

class MySocketAcceptor : public Poco::Net::SocketAcceptor<MyConnectionHandler> {
public:
    MySocketAcceptor(Poco::Net::ServerSocket& socket, 
    Poco::Net::SocketReactor& reactor, 
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections, 
    std::mutex& connectionsMutex);

protected:
    MyConnectionHandler* createServiceHandler(Poco::Net::StreamSocket& socket);

private:
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections;
    std::mutex& connectionsMutex;
};