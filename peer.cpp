#include "peer.hpp" 
#include <iostream>

#include "Poco/Net/SocketReactor.h"

Peer::Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses) :
reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor), id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << Hash(id).toString() << std::endl;


    for (const auto& remoteAddress : remoteAddresses) {
        connectors.push_back(std::make_unique<MySocketConnector>(remoteAddress, reactor));
    }

    // different thread for the reactor
    thread.start(reactor);
}

Peer::Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress) :
reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor), id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << id.toString() << std::endl;


    connectors.push_back(std::make_unique<MySocketConnector>(remoteAddress, reactor));

    // different thread for the reactor
    
    thread.start(reactor);
    
}


Peer::Peer(Poco::Net::SocketAddress ownAddress) : 
reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor), id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer has address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << id.toString() << std::endl;

    thread.start(reactor);
}

Hash Peer::getHash() const {
    return id;
}