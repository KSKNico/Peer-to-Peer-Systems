#include "peer.hpp" 
#include <iostream>

Peer::Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress) {
    // Initialize the server socket
    serverSocket = Poco::Net::ServerSocket(ownAddress);

    address = serverSocket.address();
    id = hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << hashToString(id) << std::endl;


}


Hash Peer::getHash() const {
    return id;
}