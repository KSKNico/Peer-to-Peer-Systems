#include <iostream>
#include <future>
#include "peerConfig.hpp"
#include "peer.hpp"
#include "Poco/Net/SocketAddress.h"
#include "sectorHandling/resultHandler.hpp"
#include "sectorHandling/sectorHandler.hpp"
#include "sectorHandling/primeCalculation.hpp"


int main() {
    resultHandler::initialize();
    ///*
    vector<unsigned long long> testVector0 = primeCalculation::calculatePrimes(0,1000);
    sectorHandler::handleSectorResultCalculated(testVector0,0,1000);
    resultHandler::printStuff();
    vector<unsigned long long> testVector1 = primeCalculation::calculatePrimes(1000,2000);
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000);
    resultHandler::printStuff();
    vector<unsigned long long> testVector2 = primeCalculation::calculatePrimes(2000,3000);
    sectorHandler::handleSectorResultCalculated(testVector2,2000,3000);
    resultHandler::printStuff();
    vector<unsigned long long> testVector3 = primeCalculation::calculatePrimes(9000,10000);
    sectorHandler::handleSectorResultCalculated(testVector3,9000,10000);
    resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000);
    resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000);
    resultHandler::printStuff();

     //*/
    cout << "\n\n";

    auto peer_addr_1 = Poco::Net::SocketAddress("127.0.0.1:5001");
    auto peer_addr_2 = Poco::Net::SocketAddress("127.0.0.1:5002");
    auto peer_addr_3 = Poco::Net::SocketAddress("127.0.0.1:5003");

    Peer peer_1(peer_addr_1);
    Peer peer_3(peer_addr_3);
    Peer peer_2(peer_addr_2, {peer_addr_1, peer_addr_3});



    while(1) {}
}