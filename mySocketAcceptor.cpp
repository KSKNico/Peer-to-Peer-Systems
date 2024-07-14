#include "mySocketAcceptor.hpp"

MySocketAcceptor::MySocketAcceptor(Poco::Net::ServerSocket& serverSocket, 
    Poco::Net::SocketReactor& reactor, 
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections, 
    std::mutex& connectionsMutex) : Poco::Net::SocketAcceptor<MyConnectionHandler>(serverSocket, reactor),
        connections(connections),
        connectionsMutex(connectionsMutex) {}

MyConnectionHandler* MySocketAcceptor::createServiceHandler(Poco::Net::StreamSocket& socket) {
    // change port to static port of 5000
    Poco::Net::SocketAddress address(socket.address().host().toString(), 5000);
    auto hash = Hash::hashSocketAddress(address);
    MyConnectionHandler *connectionHandler = new MyConnectionHandler(socket, *reactor());
    // add the connection to the connections map
    std::unique_lock<std::mutex> lock(connectionsMutex);
    connections.insert(std::make_pair(hash, connectionHandler));
    return connectionHandler;
}