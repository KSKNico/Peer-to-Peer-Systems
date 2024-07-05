#include "mySocketConnector.hpp"

MySocketConnector::MySocketConnector(const Poco::Net::SocketAddress& address, 
    Poco::Net::SocketReactor& reactor,
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections,
    std::mutex& connectionsMutex
    ) : 
Poco::Net::SocketConnector<MyConnectionHandler>(address, reactor),
connections(connections),
connectionsMutex(connectionsMutex)
 {}

MyConnectionHandler* MySocketConnector::createServiceHandler() {
    std::unique_lock<std::mutex> lock(connectionsMutex);
    Hash hash = Hash::hashSocketAddress(socket().peerAddress());
    MyConnectionHandler *connectionHandler = new MyConnectionHandler(socket(), *reactor());
    connections[hash] = connectionHandler;
    return connectionHandler;
}