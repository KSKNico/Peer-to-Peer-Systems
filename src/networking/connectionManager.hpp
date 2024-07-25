#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "../hash.hpp"
#include "acceptor.hpp"
#include "connection.hpp"
#include "connector.hpp"

using ConnectionsMap =
    std::unordered_map<Poco::Net::SocketAddress, std::unique_ptr<Connection>, Hash::SocketAddressHasher>;

using MessageMap = std::unordered_map<Poco::Net::SocketAddress, std::queue<Message>, Hash::SocketAddressHasher>;

class ConnectionManager {
   public:
    ConnectionManager(Poco::Net::SocketAddress ownAddress);
    void acceptAllConnections();

    bool isConnectionEstablished(const Poco::Net::SocketAddress& address) const;

    void connectTo(const Poco::Net::SocketAddress& address);

    void updateOutgoingConnections();
    void updateIncomingConnections();

   private:
    const Poco::Net::SocketAddress ownAddress;
    ConnectionsMap pendingOutgoingConnections;
    ConnectionsMap pendingIncomingConnections;

    // established connections are connections where an ID exchange has been completed
    ConnectionsMap establishedConnections;
    Acceptor acceptor;
    Connector connector;
};