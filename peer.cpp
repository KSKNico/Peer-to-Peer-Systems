#include "peer.hpp"
#include <iostream>

#include "Poco/Net/SocketReactor.h"

Peer::Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses) :
        reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor, connections, connectionsMutex),
        id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << Hash(id).toString() << std::endl;


    for (const auto &remoteAddress: remoteAddresses) {
        // connectors is an unordered_map of unique_ptrs to MySocketConnectors
        connectors[Hash::hashSocketAddress(remoteAddress)] = std::make_unique<MySocketConnector>(remoteAddress, reactor,
                                                                                                 connections,
                                                                                                 connectionsMutex);
    }

    // different thread for the reactor
    thread.start(reactor);
}

Peer::Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress) :
        reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor, connections, connectionsMutex),
        id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer created with address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << id.toString() << std::endl;


    connectors[Hash::hashSocketAddress(remoteAddress)] = std::make_unique<MySocketConnector>(remoteAddress, reactor,
                                                                                             connections,
                                                                                             connectionsMutex);

    // different thread for the reactor

    thread.start(reactor);

}

Peer::Peer(Poco::Net::SocketAddress ownAddress) :
        reactor(), serverSocket(ownAddress), acceptor(serverSocket, reactor, connections, connectionsMutex),
        id(Hash::hashSocketAddress(ownAddress)) {
    address = serverSocket.address();
    id = Hash::hashSocketAddress(address);

    std::cout << "Peer has address: " << address.toString() << std::endl;
    std::cout << "Peer has hash: " << id.toString() << std::endl;

    thread.start(reactor);
}

Hash Peer::getHash() const {
    return id;
}

void process_get_message(Message message, std::unordered_map<unsigned long long, std::vector<unsigned long long>> prime_intervals) {
    Message::get_message message_info = message.decode_get_message();
    if(prime_intervals.contains(message_info.start_of_interval)){      // if peer stores the primes of the requested interval
        std::vector<unsigned long long> result = prime_intervals[message_info.start_of_interval];
        // TODO: send a put message to the peer that requested the result
        std::cout << "sending results" << std::endl;
    } else{
        // TODO: send a put message without content
        std::cout << "sending empty put message" << std::endl;
    }
}

void process_put_message(Message message, std::unordered_map<unsigned long long, std::vector<unsigned long long>> prime_intervals) {
    Message::put_message message_info = message.decode_put_message();

    if(message_info.primes.empty()){
        std::cout << "empty prime vector, nothing to store in map" << std::endl;
        return;
    }
    if(prime_intervals.contains(message_info.start_of_interval)){      // if peer already has results for this interval
        std::vector<unsigned long long> result = prime_intervals[message_info.start_of_interval];
        if(!(message_info.primes.size() == result.size() && std::equal(message_info.primes.begin(), message_info.primes.end(), result.begin()))){
            std::cout << "different results for same interval" << std::endl;
        }
    } else{
        prime_intervals[message_info.start_of_interval] = message_info.primes;
        std::cout << "updated map with results" << std::endl;
    }
}

void Peer::processMessage(Message message) {
    switch (message.type) {
        case Message::MessageType::GET:
            process_get_message(message, prime_intervals);
        case Message::MessageType::PUT:
            process_put_message(message, prime_intervals);
        case Message::MessageType::JOIN:
            break;
        default:
            std::cout << "Message from an unknown type, ignore it.";
            break;
    }
}

void Peer::run() {
    while (true) {

        std::unique_lock<std::mutex> connectionsLock(connectionsMutex);
        for (auto &connection: connections) {
            Message message = connection.second->ioInterface.dequeueIncomingMessage();
            if (!message.isEmpty()) processMessage(message);
        }
        connectionsLock.unlock();
    }
}