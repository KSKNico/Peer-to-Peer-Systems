#include "connector.hpp"

std::unique_ptr<Connection> Connector::connect(const Poco::Net::SocketAddress &address, bool blocking) {
    Poco::Net::StreamSocket socket;

    if (!blocking) {
        socket.setBlocking(blocking);
    }
    socket.connect(address);

    return std::make_unique<Connection>(std::move(socket));
}