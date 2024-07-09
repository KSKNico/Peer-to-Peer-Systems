#include "myConnectionHandler.hpp"
#include <iostream>
#include <Poco/Buffer.h>
#include "message.hpp"

MyConnectionHandler::MyConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor& reactor) : _socket(socket), _reactor(reactor), buffer(Message::FIXED_MESSAGE_SIZE) {
    _reactor.addEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::ReadableNotification>(*this, &MyConnectionHandler::onReadable));
    _reactor.addEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::WritableNotification>(*this, &MyConnectionHandler::onWritable));

}

MyConnectionHandler::~MyConnectionHandler() {
    _reactor.removeEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::ReadableNotification>(*this, &MyConnectionHandler::onReadable));
    _reactor.removeEventHandler(_socket, Poco::Observer<MyConnectionHandler, Poco::Net::WritableNotification>(*this, &MyConnectionHandler::onWritable));
}

void MyConnectionHandler::onReadable(Poco::Net::ReadableNotification* pNf) {
    pNf->release();

    int byteCount = _socket.receiveBytes(buffer);

    if (buffer.used() == Message::FIXED_MESSAGE_SIZE) {
        // we have one full message
        auto msg = Message::fromBuffer(buffer);
        buffer.drain();
        ioInterface.queueIncomingMessage(msg);
    }
}

void MyConnectionHandler::onWritable(Poco::Net::WritableNotification* pNf) {
    pNf->release();
    
    if (ioInterface.getOutgoingMessageCount() > 0){
        auto msg = ioInterface.dequeueOutgoingMessage();
        auto _buffer = Message::toBuffer(msg);
        _socket.sendBytes(_buffer.begin(), _buffer.sizeBytes());
    }
}