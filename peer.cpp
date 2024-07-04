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

void Peer::process_get_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::get_message message_info = message.decode_get_message();
    Message::MessageData data{};

    std::string ip = address.toString();
    std::string interval = std::to_string(message_info.start_of_interval);
    std::string all = "PUT," + ip + "," + interval;

    if (prime_intervals.contains(message_info.start_of_interval)) {      // if peer stores the primes of the requested interval
        std::vector<unsigned long long> result = prime_intervals[message_info.start_of_interval];
        for (auto prime: result) {
            std::string str = std::to_string(prime);
            all += "," + str;
        }
        std::cout << "sending results" << std::endl;
    } else {
        std::cout << "sending empty put message" << std::endl;
    }
    std::strncpy(data.data(), all.c_str(), data.size());
    Message return_message(data);
    connection.second->ioInterface.queueOutgoingMessage(return_message);
}

void Peer::process_put_message(Message message) {
    Message::put_message message_info = message.decode_put_message();

    if (message_info.primes.empty()) {
        std::cout << "empty prime vector, nothing to store in map" << std::endl;
        return;
    }
    if (prime_intervals.contains(message_info.start_of_interval)) {      // if peer already has results for this interval
        std::vector<unsigned long long> result = prime_intervals[message_info.start_of_interval];
        if (!(message_info.primes.size() == result.size() &&
              std::equal(message_info.primes.begin(), message_info.primes.end(), result.begin()))) {
            std::cout << "different results for same interval" << std::endl;
        }
    } else {
        prime_intervals[message_info.start_of_interval] = message_info.primes;
        std::cout << "updated map with results" << std::endl;
    }
}

void Peer::process_join_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::join_message msg = message.decode_join_message();
    Message::MessageData data{};

    // TODO implement findClosestPeer
    std::string closestIP = findClosestPeer(msg.IP_address);
    // TODO add peer to finger table?

    std::string ip = address.toString();
    std::string fullMessage = "JOINACK," + ip + "," + closestIP;

    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);
    connection.second->ioInterface.queueOutgoingMessage(ans);
}

void Peer::process_joinack_message(Message message) {
    Message::joinack_message msg = message.decode_joinack_message();
    Message::MessageData data{};

    // TODO send SUCC message to msg.ClosestKnownIP
}

void Peer::process_succ_message(Message message, std::pair<const Hash, MyConnectionHandler *> connection) {
    Message::succ_message msg = message.decode_succ_message();
    Message::MessageData data{};

    // TODO implement findClosestPeer
    std::string closestIP = findClosestPeer(msg.IP_address);
    // TODO add peer to finger table?

    std::string ip = address.toString();

    if(closestIP == ip) {
        // set closestIP to successor

        // set successor to msg.IP_address
    }

    std::string fullMessage = "SUCCACK," + ip + "," + closestIP;

    std::strncpy(data.data(), fullMessage.c_str(), data.size());
    Message ans(data);
    connection.second->ioInterface.queueOutgoingMessage(ans);
}

void Peer::process_succack_message(Message message) {
    Message::succack_message msg = message.decode_succack_message();
    Message::MessageData data{};

    if (comesAfterMe(msg.ClosestKnownIP)) {
        // set successor to msg.IP_address
    } else {
        // TODO send SUCC message to msg.ClosestKnownIP
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
    } else {
        std::cout << "Message from an unknown type, ignore it.";
    }
}

void Peer::stabilize() {
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

void Peer::run() {
    while (true) {

        std::unique_lock<std::mutex> connectionsLock(connectionsMutex);
        for (auto &connection: connections) {
            Message message = connection.second->ioInterface.dequeueIncomingMessage();
            if (!message.isEmpty()) processMessage(message, connection);
        }
        connectionsLock.unlock();
    }
}