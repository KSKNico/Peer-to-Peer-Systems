#include "myConnectionHandler.hpp"
#include <iostream>

MyConnectionHandler::MyConnectionHandler(const Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : _socket(socket), _reactor(reactor) {
    _reactor.addEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::ReadableNotification>(*this, &MyConnectionHandler::onReadable));
    _reactor.addEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::WritableNotification>(*this, &MyConnectionHandler::onWritable));
}

MyConnectionHandler::~MyConnectionHandler() {
    _reactor.removeEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::ReadableNotification>(*this, &MyConnectionHandler::onReadable));
    _reactor.removeEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::WritableNotification>(*this, &MyConnectionHandler::onWritable));
}

void MyConnectionHandler::onReadable(Poco::Net::ReadableNotification* pNf) {
    std::cout << "Received data" << std::endl;
}

void MyConnectionHandler::onWritable(Poco::Net::WritableNotification* pNf) {
    std::cout << "Ready to send data" << std::endl;
}