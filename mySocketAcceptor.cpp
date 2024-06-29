#include "mySocketAcceptor.hpp"

MySocketAcceptor::MySocketAcceptor(Poco::Net::ServerSocket& socket, Poco::Net::SocketReactor& reactor) : Poco::Net::SocketAcceptor<MyConnectionHandler>(socket, reactor) {}

MyConnectionHandler* MySocketAcceptor::createServiceHandler(Poco::Net::StreamSocket& socket) {
    return new MyConnectionHandler(socket, *reactor());
}