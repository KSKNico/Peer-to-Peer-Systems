#include <iostream>
#include <future>
#include "peerConfig.hpp"
#include "peer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "sectorHandling/resultHandler.hpp"
#include "sectorHandling/sectorHandler.hpp"
#include "sectorHandling/primeCalculation.hpp"


int main() {

    //std::future<vector<unsigned long long >> testFuture =  primeCalculation::calculateCurrentSector();
    //vector<unsigned long long> result = testFuture.get();

    resultHandler::initialize();


    sectorHandler::findResultLocally(0);
    sectorHandler::findResultLocally(1000);
    sectorHandler::findResultLocally(2000);
    sectorHandler::findResultLocally(3000);

    vector<unsigned long long> testVector = primeCalculation::calculatePrimes(0,1000);
    resultHandler::saveResultLocally(testVector,0,1000);


    cout << "\n\n";

    auto peer_addr_1 = Poco::Net::SocketAddress("127.0.0.1:5001");
    auto peer_addr_2 = Poco::Net::SocketAddress("127.0.0.1:5002");
    auto peer_addr_3 = Poco::Net::SocketAddress("127.0.0.1:5003");

    Peer peer_1(peer_addr_1);
    Peer peer_3(peer_addr_3);
    Peer peer_2(peer_addr_2, {peer_addr_1, peer_addr_3});



    while(1) {}
}