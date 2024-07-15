#include "mySocketConnector.hpp"
#include <iostream>

MySocketConnector::MySocketConnector(const Poco::Net::SocketAddress& address, 
    Poco::Net::SocketReactor& reactor,
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher>& connections,
    std::mutex& connectionsMutex
    ) : 
Poco::Net::SocketConnector<MyConnectionHandler>(address, reactor),
connections(connections),
connectionsMutex(connectionsMutex),
finished(false),
remoteAddress(address)
 {}

MyConnectionHandler* MySocketConnector::createServiceHandler() {
    std::cout << "Connection established to: " << remoteAddress.host().toString() << std::endl;
    auto connectionHandler = new MyConnectionHandler(Poco::Net::SocketConnector<MyConnectionHandler>::socket(), 
                                    *Poco::Net::SocketConnector<MyConnectionHandler>::reactor());
    auto hash = Hash::hashSocketAddress(remoteAddress);
    std::unique_lock<std::mutex>(connectionsMutex);
    connections.insert(std::make_pair(hash, connectionHandler));
    this->finished = true;
    return connectionHandler;
}

void MySocketConnector::onError(int errorCode) {
    std::cout << "Connection failed to " << remoteAddress.host().toString() << " with error code " <<
    errorCode << std::endl;
}

bool MySocketConnector::isFinished() const {
    return finished;
}