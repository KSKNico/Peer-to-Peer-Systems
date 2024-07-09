#include "peer.hpp"
#include <iostream>

#include "Poco/Net/SocketReactor.h"
#include "sectorHandling/sectorHandler.hpp"

/* Peer::Peer(Poco::Net::SocketAddress ownAddress, std::vector<Poco::Net::SocketAddress> remoteAddresses) :
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
} */

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

    while(connections.empty()){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto data = Message::MessageData{};
    std::string str = "JOIN," + address.toString();
    std::strncpy(data.data(), str.c_str(), data.size());
    connections[Hash::hashSocketAddress(remoteAddress)]->ioInterface.queueOutgoingMessage(Message(data));
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

void Peer::process_get_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::get_message message_info = message.decode_get_message();
    Message::MessageData data{};

    std::string ip = address.toString();
    std::string interval = std::to_string(message_info.start_of_interval);
    std::string all = "PUT," + ip + "," + interval;

    //updating results for safety reasons
    unordered_map<unsigned long long, vector<unsigned long long>> localResults = sectorHandler::getAllResults();
    for (auto & localResult : localResults) {
        unsigned long long key = localResult.first;
        vector<unsigned long long > value = localResult.second;
        prime_intervals.operator[](key) = value;
        //cout << (prime_intervals.at(key)[0]);
    }

    if (prime_intervals.contains(message_info.start_of_interval)) {
        // if peer stores the primes of the requested interval
        //std::vector<unsigned long long> result = prime_intervals[message_info.start_of_interval];

        std::vector<unsigned long long> result = get<0>(sectorHandler::findResultLocally(message_info.start_of_interval));
        for (auto prime: result) {
            std::string str = std::to_string(prime);
            all += "," + str;
        }
        std::cout << "sending results" << std::endl;
    } else {
        Hash interval_hash = Hash::hashInterval(message_info.start_of_interval);

        std::string closestIP = findClosestPeer(interval_hash);
        if (closestIP == ip) {
            closestIP = successor.toString();
        }
        all = "GETACK," + ip + "," + interval + "," + closestIP;

        std::cout << "sending getack message" << std::endl;
    }
    std::strncpy(data.data(), all.c_str(), data.size());
    Message return_message(data);
    connection.second->ioInterface.queueOutgoingMessage(return_message);


}

void Peer::process_getack_message(Message message) {
    Message::getack_message msg = message.decode_getack_message();
    Message::MessageData data{};

    auto routing_addr = Poco::Net::SocketAddress(msg.RoutingIP);
    Hash routing_hash = Hash::hashSocketAddress(routing_addr);

    std::string ip = address.toString();
    std::string interval = std::to_string(msg.start_of_interval);
    std::string all = "GET," + ip + "," + interval;

    std::strncpy(data.data(), all.c_str(), data.size());
    Message ans(data);

    if (connections.find(routing_hash) != connections.end()) {
        connections[routing_hash]->ioInterface.queueOutgoingMessage(ans);
    } else {
        connectors[routing_hash] = std::make_unique<MySocketConnector>(routing_addr, reactor,
                                                                    connections,
                                                                    connectionsMutex);
        outgoingMessages[routing_hash].push_back(ans);
    }
}

void Peer::process_put_message(Message message) {
    Message::put_message message_info = message.decode_put_message();

    if (message_info.primes.empty()) {
        std::cout << "empty prime vector, nothing to store in map" << std::endl;
        return;
    }
    unordered_map<unsigned long long, vector<unsigned long long>> localResults = sectorHandler::getAllResults();
    for (auto & localResult : localResults) {
        unsigned long long key = localResult.first;
        vector<unsigned long long > value = localResult.second;
        prime_intervals.operator[](key) = value;
        //cout << (prime_intervals.at(key)[0]);
    }

    // if peer already has results for this interval
    if (prime_intervals.contains(message_info.start_of_interval)) {
        std::vector<unsigned long long> result = prime_intervals[message_info.start_of_interval];
        if (!(message_info.primes.size() == result.size() &&
              std::equal(message_info.primes.begin(), message_info.primes.end(), result.begin()))) {
            std::cout << "different results for same interval" << std::endl;
        }
    } else {
        prime_intervals[message_info.start_of_interval] = message_info.primes;
        sectorHandler::handleSectorResultFromPeer(message);
        std::cout << "updated map with results" << std::endl;
    }
}

void Peer::process_join_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::join_message msg = message.decode_join_message();
    Message::MessageData data{};

    auto peerIP = Poco::Net::SocketAddress(msg.IP_address);
    Hash peerPosition = Hash::hashSocketAddress(peerIP);

    std::string closestIP = findClosestPeer(peerPosition);

    std::string ip = address.toString();
    std::string fullMessage = "JOINACK," + ip + "," + closestIP;

    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);
    connection.second->ioInterface.queueOutgoingMessage(ans);
}

void Peer::process_joinack_message(Message message) {
    Message::joinack_message msg = message.decode_joinack_message();
    Message::MessageData data{};

    auto peer_addr = Poco::Net::SocketAddress(msg.ClosestKnownIP);
    Hash peer_hash = Hash::hashSocketAddress(peer_addr);

    predecessor = peer_addr;

    std::string ip = address.toString();
    std::string fullMessage = "SUCC," + ip;
    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);

    if (connections.find(peer_hash) != connections.end()) {
        connections[peer_hash]->ioInterface.queueOutgoingMessage(ans);
    } else {
        connectors[peer_hash] = std::make_unique<MySocketConnector>(peer_addr, reactor,
                                                                    connections,
                                                                    connectionsMutex);
        outgoingMessages[peer_hash].push_back(ans);
    }
}

std::string Peer::findClosestPeer(Hash& position) {
    Hash closestPeer = Hash::hashSocketAddress(address);

    for (const auto& entry: fingerTable) {
        if ((entry.first.isBefore(position)) && (!entry.first.isBefore(closestPeer))) {
            closestPeer = entry.first;
        }
    }

    return closestPeer.toString();
}

void Peer::process_succ_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::succ_message msg = message.decode_succ_message();
    Message::MessageData data{};

    Hash myPosition = Hash::hashSocketAddress(address);
    auto peerIP = Poco::Net::SocketAddress(msg.IP_address);
    Hash peerPosition = Hash::hashSocketAddress(peerIP);

    std::string succ = successor.toString();  // send IP of old successor even if it changes
    std::string ip = address.toString();

    if(successor == address || myPosition.isBefore(peerPosition) && peerPosition.isBefore(Hash::hashSocketAddress(successor))){
        // I have no succ or this peer is between my succ and me -> this peer will be my new succ
        successor = peerIP;
    }
    if(predecessor == address) predecessor = peerIP;    // if I'm the only peer, I was my pred -> new peer is not only my succ but also my pred

    std::string fullMessage = "SUCCACK," + ip + "," + succ;

    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);

    connection.second->ioInterface.queueOutgoingMessage(ans);
}

void Peer::process_succack_message(Message message) {
    Message::succack_message msg = message.decode_succack_message();
    Message::MessageData data{};

    Hash myPosition = Hash::hashSocketAddress(address);
    auto peerIP = Poco::Net::SocketAddress(msg.IP_address);
    Hash peerPosition = Hash::hashSocketAddress(peerIP);
    auto closestIP = Poco::Net::SocketAddress(msg.ClosestKnownIP);
    Hash closestIPPosition = Hash::hashSocketAddress(closestIP);

    if (peerPosition.isBefore(myPosition) && myPosition.isBefore(closestIPPosition)) {
        // if I'm between peers successor and peer -> peers succ will be my succ and peer will be my pred
        successor = closestIP;
        predecessor = peerIP;
    } else if(closestIP == address) return; // if I'm the successor, nothing to do
    else if(peerIP == closestIP) predecessor = successor = peerIP;  // peer has itself as successor -> we are the only 2 peers in the system -> we are pred and succ for each other
}

void Peer::process_pred_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::pred_message msg = message.decode_pred_message();
    Message::MessageData data{};

    auto peerIP = Poco::Net::SocketAddress(msg.IP_address);
    Hash peerPosition = Hash::hashSocketAddress(peerIP);

    std::string pred = predecessor.toString();  // send IP of old predecessor even if it changes
    std::string ip = address.toString();

    if(predecessor == address || peerPosition.isBefore(Hash::hashSocketAddress(address)) && Hash::hashSocketAddress(predecessor).isBefore(peerPosition)){
        // I have no pred or this peer is between my pred and me -> this peer will be my new pred
        predecessor = peerIP;
    }
    if(successor == address) successor = peerIP;    // if I'm the only peer, I was my succ -> new peer is not only my pred but also my succ

    std::string fullMessage = "PREDACK," + ip + "," + pred;

    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);

    connection.second->ioInterface.queueOutgoingMessage(ans);
}

void Peer::process_predack_message(Message message) {
    Message::predack_message msg = message.decode_predack_message();
    Message::MessageData data{};

    Hash myPosition = Hash::hashSocketAddress(address);
    auto peerIP = Poco::Net::SocketAddress(msg.IP_address);
    Hash peerPosition = Hash::hashSocketAddress(peerIP);
    auto currentPred = Poco::Net::SocketAddress(msg.currentPred);
    Hash currentPredPosition = Hash::hashSocketAddress(currentPred);

    if(currentPred.toString() == msg.IP_address){
        // peer was the only one -> we are just 2 peers in this network -> we are pred and succ for us
        successor = predecessor = peerIP;
    } else if(currentPred == address){
        return; // if I'm already the predecessor, there is nothing to do
    } else if(currentPredPosition.isBefore(myPosition) && myPosition.isBefore(peerPosition)){
        // I'm between the pred of the peer and the peer -> this peer will be my succ and its pred will be my pred
        successor = peerIP;
        predecessor = currentPred;
    }
}

void Peer::process_fing_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::fing_message msg = message.decode_fing_message();
    Message::MessageData data{};

    std::string ip = address.toString();
    std::string succ = successor.toString();
    std::string fullMessage = "FINGACC," + ip + "," + succ;
    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);

    connection.second->ioInterface.queueOutgoingMessage(ans);
}

void Peer::process_fingack_message(Message message) {
    Message::fingack_message msg = message.decode_fingack_message();
    Message::MessageData data{};

    auto fing_addr = Poco::Net::SocketAddress(msg.SuccessorIP);
    Hash fing_hash = Hash::hashSocketAddress(fing_addr);
    fingerTable[fing_hash] = fing_addr;

    Hash me_hash = Hash(Hash::hashSocketAddress(address));
    std::string half = "7fffffffffffffff";
    Hash half_hash = Hash::fromString(half);

    // if the finger is less than half a circle away, request his successor
    if ((fing_hash - me_hash) < half_hash) {

        std::string ip = address.toString();
        std::string fullMessage = "FING," + ip;
        std::strncpy(data.data(), fullMessage.c_str(), data.size());
        Message ans(data);

        if (connections.find(fing_hash) != connections.end()) {
            connections[fing_hash]->ioInterface.queueOutgoingMessage(ans);
        } else {
            connectors[fing_hash] = std::make_unique<MySocketConnector>(fing_addr, reactor,
                                                                        connections,
                                                                        connectionsMutex);
            outgoingMessages[fing_hash].push_back(ans);
        }
    }
}

void Peer::processMessage(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    std::string message_type(message.data.data(), message.data.size());
    size_t pos = message_type.find(',');

    if (pos == std::string::npos) {
        std::cout << "No ',' in message with type" << std::endl;
        return;
    }

    std::string data_without_type = message_type.substr(pos + 1);

    std::strncpy(message.data.data(), data_without_type.c_str(), message.data.size());

    pos = data_without_type.find(',');

    if (pos == std::string::npos) {
        std::cout << "No ',' in message without type" << std::endl;
        return;
    }

    if (message_type.substr(0, pos) == "GET") {
        message.type = Message::MessageType::GET;
        process_get_message(message, connection);
    } else if (message_type.substr(0, pos) == "PUT") {
        message.type = Message::MessageType::PUT;
        process_put_message(message);
    } else if (message_type.substr(0, pos) == "JOIN") {
        message.type = Message::MessageType::JOIN;
        process_join_message(message, connection);
    } else if ((message_type.substr(0, pos) == "JOINACK")) {
        message.type = Message::MessageType::JOINACK;
        process_joinack_message(message);
    } else if ((message_type.substr(0, pos) == "SUCC")) {
        message.type = Message::MessageType::SUCC;
        process_succ_message(message, connection);
    } else if ((message_type.substr(0, pos) == "SUCCACK")) {
        message.type = Message::MessageType::SUCCACK;
        process_succack_message(message);
    } else if ((message_type.substr(0, pos) == "PRED")) {
        message.type = Message::MessageType::PRED;
        process_pred_message(message, connection);
    } else if ((message_type.substr(0, pos) == "PREDACK")) {
        message.type = Message::MessageType::PREDACK;
        process_predack_message(message);
    } else if ((message_type.substr(0, pos) == "FING")) {
        message.type = Message::MessageType::FING;
        process_fing_message(message, connection);
    } else if ((message_type.substr(0, pos) == "FINGACK")) {
        message.type = Message::MessageType::FINGACK;
        process_fingack_message(message);
    } else if ((message_type.substr(0, pos) == "GETACK")) {
        message.type = Message::MessageType::GETACK;
        process_getack_message(message);
    } else {
        std::cout << "Message from an unknown type, ignore it.";
    }
}

void Peer::stabilize() {
    Message::MessageData data{};
    std::string str = "PRED," + address.toString();

    std::strncpy(data.data(), str.c_str(), data.size());

    Message message(data);

    MyConnectionHandler *handler = connections[Hash::hashSocketAddress(successor)];

    if(handler == nullptr){
        std::cout << "successor address not found" << std::endl;
        return;
    }

    handler->ioInterface.queueOutgoingMessage(message);
    /*
     * send message to successor -> "who is your predecessor"
     *
     * if (predecessor not me)
     *  updateSuccessor(predecessor)
     *  send message to successor -> "im now your predecessor"
     *  await response -> "no my predecessor is closer" | "ok"
     *
     */
}


void Peer::initFingerTable(MyConnectionHandler * successorConnection) {
    // sends the initial FING message to the successor
    std::string ip = address.toString();
    std::string fullMessage = "FING," + ip;
    Message::MessageData data{};
    std::strncpy(data.data(), fullMessage.c_str(), data.size());

    Message message(data);

    successorConnection->ioInterface.queueOutgoingMessage(message);
}

void Peer::run() {
    initFingerTable(connections[Hash::hashSocketAddress(successor)]);
    while (true) {

        std::unique_lock<std::mutex> connectionsLock(connectionsMutex);
        for (auto &connection: connections) {
            Message message = connection.second->ioInterface.dequeueIncomingMessage();
            if (!message.isEmpty()) processMessage(message, connection);

            auto it = outgoingMessages.find(connection.first);
            if (it != outgoingMessages.end() && !it->second.empty()) {
                for (const auto& msg : it->second) {
                    connection.second->ioInterface.queueOutgoingMessage(msg);
                }
                outgoingMessages.erase(it);
            }
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