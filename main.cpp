#include <iostream>
#include <future>
#include "peerConfig.hpp"
#include "peer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "primeCalculation.hpp"




int main() {

    std::future<vector<unsigned long long >> testFuture =  primeCalculation::calculateCurrentSector();
    vector<unsigned long long> result = testFuture.get();
    /*
    for (auto v : result) {
        cout << v << " ";
    }
    */
    auto peer_addr_1 = Poco::Net::SocketAddress("127.0.0.1:5001");
    auto peer_addr_2 = Poco::Net::SocketAddress("127.0.0.1:5002");
    auto peer_addr_3 = Poco::Net::SocketAddress("127.0.0.1:5003");

    Peer peer_1(peer_addr_1);
    Peer peer_3(peer_addr_3);
    Peer peer_2(peer_addr_2, {peer_addr_1, peer_addr_3});



    while(1) {}
}