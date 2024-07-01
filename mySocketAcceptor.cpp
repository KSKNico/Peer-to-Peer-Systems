#include "mySocketAcceptor.hpp"

MySocketAcceptor::MySocketAcceptor(Poco::Net::ServerSocket& socket, 
    Poco::Net::SocketReactor& reactor, 
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections, 
    std::mutex& connectionsMutex) : Poco::Net::SocketAcceptor<MyConnectionHandler>(socket, reactor),
        connections(connections),
        connectionsMutex(connectionsMutex) {}

MyConnectionHandler* MySocketAcceptor::createServiceHandler(Poco::Net::StreamSocket& socket) {
    std::unique_lock<std::mutex> lock(connectionsMutex);
    Hash hash = Hash::hashSocketAddress(socket.peerAddress());
    MyConnectionHandler *connectionHandler = new MyConnectionHandler(socket, *reactor());
    // add the connection to the connections map
    connections[hash] = connectionHandler;
    return connectionHandler;
}