#pragma once

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "message.hpp"

class Connection {
   public:
    Connection(Poco::Net::StreamSocket&& socket);

    void sendMessage(Message message);
    void sendMessages(std::vector<Message> messages);
    std::optional<Message> receiveMessage();
    std::vector<Message> receiveMessages();
    bool isConnected();
    bool isReadable();
    bool isWritable();

   private:
    Poco::Net::StreamSocket socket;
    Poco::FIFOBuffer inputBuffer;
    Poco::FIFOBuffer outputBuffer;
    Poco::Net::SocketStream stream;
};