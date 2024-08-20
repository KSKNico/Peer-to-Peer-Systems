#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "../hash.hpp"
#include "acceptor.hpp"
#include "connection.hpp"
#include "connector.hpp"

using ConnectionPair = std::pair<Poco::Net::SocketAddress, std::unique_ptr<Connection>>;

using ConnectionsMap =
    std::unordered_map<Poco::Net::SocketAddress, std::unique_ptr<Connection>, Hash::SocketAddressHasher>;

// using MessageMap = std::unordered_map<Poco::Net::SocketAddress, std::queue<Message>, Hash::SocketAddressHasher>;

class ConnectionManager {
   public:
    ConnectionManager() = delete;
    ConnectionManager(Poco::Net::SocketAddress ownAddress);
    void acceptAllConnections();

    bool isConnectionEstablished(const Poco::Net::SocketAddress& address) const;

    void connectTo(const Poco::Net::SocketAddress& address);
    void sendMessage(const Poco::Net::SocketAddress& address, const Message& message);
    void closeConnection(const Poco::Net::SocketAddress& address);
    std::vector<MessagePair> receiveMessages();

    // removes all connections that are errored or closed
    // and returns a list of addresses that were removed
    std::vector<Poco::Net::SocketAddress> checkEstablishedConnections();

    void update();

    // only connect if the connection does not exist yet
    bool existsElseConnect(const Poco::Net::SocketAddress& address);

    void updateOutgoingConnections();
    void updateIncomingConnections();

    std::size_t getEstablishedConnectionsCount() const;

    std::vector<Poco::Net::SocketAddress> getEstablishedConnections() const;

   private:
    const Poco::Net::SocketAddress ownAddress;
    ConnectionsMap pendingOutgoingConnections;
    ConnectionsMap pendingIncomingConnections;

    // established connections are connections where an ID exchange has been completed
    ConnectionsMap establishedConnections;
    Acceptor acceptor;
    Connector connector;
};