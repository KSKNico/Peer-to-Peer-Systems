#pragma once

#include <unordered_map>

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "message.hpp"

using MessagePair = std::pair<Poco::Net::SocketAddress, std::unique_ptr<Message>>;

class Connection {
   public:
    Connection(Poco::Net::StreamSocket&& socket);

    void sendMessage(const Message& message);
    void sendMessages(const std::vector<Message>& messages);
    std::unique_ptr<Message> receiveMessage();
    bool isConnected();
    bool isReadable();
    bool isWritable();
    Poco::Net::SocketAddress getOwnAddress() const;
    Poco::Net::SocketAddress getPeerAddress() const;

    // should not be called
    void _sendString(const std::string& str);

    // delete copy constructor and copy assignment operator
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

   private:
    Poco::Net::StreamSocket socket;
    Poco::Net::SocketStream stream;
    std::string remaining;
    std::string str;
};