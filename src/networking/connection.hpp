#pragma once

#include <unordered_map>

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
    Poco::Net::SocketAddress getOwnAddress() const;
    Poco::Net::SocketAddress getPeerAddress() const;

   private:
    Poco::Net::StreamSocket socket;
    Poco::FIFOBuffer inputBuffer;
    Poco::FIFOBuffer outputBuffer;
    Poco::Net::SocketStream stream;
};

using ConnectionsMap =
    std::unordered_map<Poco::Net::SocketAddress, std::unique_ptr<Connection>, Hash::SocketAddressHasher>;