#include "peer.hpp" 
#include <iostream>

#include "Poco/Net/SocketReactor.h"

Peer::Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses) :
reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor, connections, connectionsMutex), id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << Hash(id).toString() << std::endl;


    for (const auto& remoteAddress : remoteAddresses) {
        // connectors is an unordered_map of unique_ptrs to MySocketConnectors
        connectors[Hash::hashSocketAddress(remoteAddress)] = std::make_unique<MySocketConnector>(remoteAddress, reactor, connections, connectionsMutex);
    }

    // different thread for the reactor
    thread.start(reactor);
}

Peer::Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress) :
reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor, connections, connectionsMutex), id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << id.toString() << std::endl;


    connectors[Hash::hashSocketAddress(remoteAddress)] = std::make_unique<MySocketConnector>(remoteAddress, reactor, connections, connectionsMutex);

    // different thread for the reactor
    
    thread.start(reactor);
    
}

Peer::Peer(Poco::Net::SocketAddress ownAddress) : 
reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor, connections, connectionsMutex), id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer has address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << id.toString() << std::endl;

    thread.start(reactor);
}

Hash Peer::getHash() const {
    return id;
}

void Peer::run() {
    while (true) {

        std::unique_lock<std::mutex> connectionsLock(connectionsMutex);
        for (auto& connection : connections) {
            connection.second->ioInterface.dequeueIncomingMessage();
        }
        connectionsLock.unlock();

        for (auto& connector : connectors) {
            if (!connector.second->isFinished()) {
                // remove it from the map
                // this also removes the mySocketConnector object as it is a unique_ptr
                connectors.erase(connector.first);
            }
        }
    }
}