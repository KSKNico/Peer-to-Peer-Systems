#include "peer.hpp" 
#include <iostream>

#include "Poco/Net/SocketReactor.h"

Peer::Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress) :
reactor(), serverSocket(ownAddress) {
    address = serverSocket.address();
    id = hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << hashToString(id) << std::endl;


    Poco::Net::SocketAcceptor<MyConnectionHandler> acceptor(serverSocket, reactor);
    Poco::Net::SocketConnector<MyConnectionHandler> connector(remoteAddress, reactor); 

    // different thread for the reactor
    
    thread.start(reactor);
    
}


Peer::Peer(Poco::Net::SocketAddress ownAddress) : reactor(), serverSocket(ownAddress) {
    address = serverSocket.address();
    id = hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << hashToString(id) << std::endl;

    Poco::Net::SocketAcceptor<MyConnectionHandler> acceptor(serverSocket, reactor);

    thread.start(reactor);
}

Hash Peer::getHash() const {
    return id;
}