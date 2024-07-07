
#include <iostream>
#include <list>
#include <vector>
#include <future>
#include <map>
#include <thread>
#include "sectorHandler.hpp"
#include "resultHandler.hpp"
#include "primeCalculation.hpp"

using namespace std;

void sectorHandler::initialize(){
    //initialize map with confirmed results and unconfirmed results
    resultHandler::initialize();
    ///* for testing purposes
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

    cout << "\n\n";
    //*/
}
future<void> sectorHandler::calculateNewSector(){
    unsigned long long lowerBound = primeCalculation::getLowerBound();
    unsigned long long upperBound = primeCalculation::getUpperBound();

    vector<unsigned long long> newCalc = primeCalculation::calculatePrimes(lowerBound,upperBound);
    sectorHandler::handleSectorResultCalculated(newCalc,lowerBound,upperBound);

    return {};
}

//handle incoming result from another peer
future<void> sectorHandler::handleSectorResultFromPeer(vector<unsigned long long> sectorResult,
                                               unsigned long long lowerBound,
                                               unsigned long long upperBound){
    resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
}

     //handles calculation we did ourselves
     //     save locally?
     //     send to responsible Peer
future<void> sectorHandler::handleSectorResultCalculated(vector<unsigned long long> sectorResult,
                                                 unsigned long long lowerBound,
                                                 unsigned long long upperBound){

    resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
    //TODO Find Peer to Send result to
}

//find Result another Peer asks for locally
tuple<vector<unsigned long long >,unsigned long long, unsigned long long > sectorHandler::findResultLocally(unsigned long long lowerBound){
         return resultHandler::findConfirmedResultLocally(lowerBound);
}

//find the highest Sector you have confirmed results for
vector<unsigned long long> sectorHandler::getHighestLocalSector(){
    //TODO
}

//get the highest confirmed Sector of a Peer
vector<unsigned long long > sectorHandler::getHighestPeerSector() {
    //TODO
}

//get specific Result from another Peer
vector<unsigned long long > sectorHandler::findResultPeer(unsigned long long sectorId){
    //TODO
}






