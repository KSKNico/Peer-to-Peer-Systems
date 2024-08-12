#include "connector.hpp"

std::unique_ptr<Connection> Connector::connect(const Poco::Net::SocketAddress &address, bool blocking) {
    Poco::Net::StreamSocket socket = Poco::Net::StreamSocket();
    // std::cout << "is blocking: " << socket.getBlocking() << std::endl;

    if (blocking) {
        socket.connect(address);
    } else {
        socket.connectNB(address);
    }

    return std::make_unique<Connection>(std::move(socket));
}