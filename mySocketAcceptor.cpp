#include "mySocketAcceptor.hpp"

MySocketAcceptor::MySocketAcceptor(Poco::Net::ServerSocket& socket) : Poco::Net::SocketAcceptor<MyConnectionHandler>(socket) {}

MyConnectionHandler* MySocketAcceptor::createServiceHandler(const Poco::Net::StreamSocket& socket) {
    return new MyConnectionHandler(socket, *reactor());
}