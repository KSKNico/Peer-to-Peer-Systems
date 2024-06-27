#include "peer.hpp" 


Peer::Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> knownHosts = {}) {
    // Initialize the server socket
    if (knownHosts.size() > 0) {
        // Connect to the known hosts, only one at the moment
        Poco::Net::StreamSocket streamSocket;
        try {
            streamSocket.connect(knownHosts[0], Poco::Timespan(1, 0));
        } catch (Poco::Net::ConnectionRefusedException &e) {
            std::cerr << "Could not connect to known host: " << knownHosts[0].toString() << std::endl;
        }
    }

    serverSocket = Poco::Net::ServerSocket(ownAddress);

    address = serverSocket.address();
    id = hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << hashToString(id) << std::endl;
}


Hash Peer::getHash() const {
    return id;
}