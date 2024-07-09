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
    std::unique_lock<std::mutex>(connectionsMutex);
    connections[
        Hash::hashSocketAddress(Poco::Net::SocketConnector<MyConnectionHandler>::socket().peerAddress())
        ] = connectionHandler;
    return connectionHandler;
}

bool MySocketConnector::isFinished() const {
    return finished;
}