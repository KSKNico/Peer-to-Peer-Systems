#pragma once

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"

#include "iointerface.hpp"

class MyConnectionHandler {
public:
    MyConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);
    ~MyConnectionHandler();
    void onReadable(Poco::Net::ReadableNotification* pNf);
    void onWritable(Poco::Net::WritableNotification* pNf);
    IOInterface ioInterface;
    Poco::Buffer<char> buffer;

private:
    Poco::Net::StreamSocket& _socket;
    Poco::Net::SocketReactor& _reactor;
};