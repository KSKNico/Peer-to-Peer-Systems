#include "connector.hpp"

std::unique_ptr<Connection> Connector::connect(const Poco::Net::SocketAddress &address) {
    Poco::Net::StreamSocket socket;
    socket.connect(address);

    return std::make_unique<Connection>(std::move(socket));
}