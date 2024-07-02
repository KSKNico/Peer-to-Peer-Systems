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
    return new MyConnectionHandler(Poco::Net::SocketConnector<MyConnectionHandler>::socket(), 
                                    *Poco::Net::SocketConnector<MyConnectionHandler>::reactor());
}