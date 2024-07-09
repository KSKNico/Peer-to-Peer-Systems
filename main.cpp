#include <iostream>
#include <future>
#include "peerConfig.hpp"
#include "peer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "sectorHandling/sectorHandler.hpp"
#include "sectorHandling/primeCalculation.hpp"



int main() {

    sectorHandler::initialize();

    auto peer_addr_1 = Poco::Net::SocketAddress("127.0.0.1:5001");
    auto peer_addr_2 = Poco::Net::SocketAddress("127.0.0.1:5002");
    auto peer_addr_3 = Poco::Net::SocketAddress("127.0.0.1:5003");
    auto peer_addr_4 = Poco::Net::SocketAddress("127.0.0.1:5004");


    Peer peer_1(peer_addr_1);
    Peer peer_3(peer_addr_3);
    Peer peer_2(peer_addr_2, {peer_addr_1,peer_addr_3});
    Peer peer_4(peer_addr_4, {peer_addr_1,peer_addr_2});

    while(1) {}
}