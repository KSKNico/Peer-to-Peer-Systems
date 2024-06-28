#pragma once

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"

class MyConnectionHandler {
public:
    MyConnectionHandler(const Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor);
    ~MyConnectionHandler();
    void onReadable(Poco::Net::ReadableNotification* pNf);
    void onWritable(Poco::Net::WritableNotification* pNf);

private:
    const Poco::Net::StreamSocket& _socket;
    Poco::Net::SocketReactor& _reactor;
};