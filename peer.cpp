#include "peer.hpp" 



Peer::Peer(int port, std::vector<Poco::Net::SocketAddress> knownHosts = {}) {
    // Initialize the server socket
    if (knownHosts.size() > 0) {
        // Connect to the known hosts
        Poco::Net::StreamSocket streamSocket;
        streamSocket.connect(knownHosts[0], Poco::Timespan(1, 0));
    }

    serverSocket = Poco::Net::ServerSocket(port);

    address = serverSocket.address();

    std::cout << "Peer created with address: " << address.toString() << std::endl; 

  
}

Hash Peer::getHash() const {
    return id;
}

Poco::Net::SocketAddress Peer::detectOwnIPAddress(Poco::Net::SocketAddress const &knownHost) const {
    return knownHost;
}