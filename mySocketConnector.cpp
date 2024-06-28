#include "mySocketConnector.hpp"

MySocketConnector::MySocketConnector(const Poco::Net::SocketAddress& address) : Poco::Net::SocketConnector<MyConnectionHandler>(address) {}

MyConnectionHandler* MySocketConnector::createServiceHandler() {
    return new MyConnectionHandler(Poco::Net::SocketConnector<MyConnectionHandler>::socket(), 
                                    *Poco::Net::SocketConnector<MyConnectionHandler>::reactor());
}