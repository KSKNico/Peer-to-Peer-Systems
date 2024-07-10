#include "mySocketConnector.hpp"

MySocketConnector::MySocketConnector(const Poco::Net::SocketAddress& address, 
    Poco::Net::SocketReactor& reactor,
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections,
    std::mutex& connectionsMutex
    ) : 
Poco::Net::SocketConnector<MyConnectionHandler>(address, reactor),
connections(connections),
connectionsMutex(connectionsMutex),
finished(false)
 {}

MyConnectionHandler* MySocketConnector::createServiceHandler() {
    auto connectionHandler = new MyConnectionHandler(Poco::Net::SocketConnector<MyConnectionHandler>::socket(), 
                                    *Poco::Net::SocketConnector<MyConnectionHandler>::reactor());
    this->finished = true;
    auto hash = Hash::hashSocketAddress(Poco::Net::SocketConnector<MyConnectionHandler>::socket().peerAddress());
    std::unique_lock<std::mutex>(connectionsMutex);
    connections.insert(std::make_pair(hash, connectionHandler));
    return connectionHandler;
}

bool MySocketConnector::isFinished() const {
    return finished;
}