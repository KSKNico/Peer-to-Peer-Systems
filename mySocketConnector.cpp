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

/*
MySocketConnector::~MySocketConnector() {
    std::cout << "MySocketConnector destructor called, this should not be called." << std::endl;
}
*/

MyConnectionHandler* MySocketConnector::createServiceHandler() {
    std::unique_lock<std::mutex>(connectionsMutex);
    auto hash = Hash::hashSocketAddress(remoteAddress);
    if (connections.contains(hash)) {
        std::cout << "Connection to " << remoteAddress.host().toString() << " already exists." << std::endl; 
        return connections.at(hash);
    }
    std::cout << "Connection established to: " << remoteAddress.host().toString() << std::endl;
    auto connectionHandler = new MyConnectionHandler(Poco::Net::SocketConnector<MyConnectionHandler>::socket(), 
                                    *Poco::Net::SocketConnector<MyConnectionHandler>::reactor());

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