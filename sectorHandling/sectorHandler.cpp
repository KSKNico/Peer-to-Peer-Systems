
#include <iostream>
#include <list>
#include <utility>
#include <vector>
#include <future>
#include <map>
#include <thread>
#include "sectorHandler.hpp"
#include "resultHandler.hpp"
#include "primeCalculation.hpp"
#include "../message.hpp"
#include "../iointerface.hpp"

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
void sectorHandler::handleSectorResultFromPeer(Message message){
    Message::put_message decodePutMessage = message.decode_put_message();
    vector<unsigned long long> sectorResult = decodePutMessage.primes;
    unsigned long long lowerBound = decodePutMessage.start_of_interval;
    unsigned long long upperBound  = lowerBound+1000;
    resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
}

     //handles calculation we did ourselves
     //     save locally
     //     send to responsible Peer
void sectorHandler::handleSectorResultCalculated(vector<unsigned long long> sectorResult,
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
tuple<vector<unsigned long long>,unsigned long, unsigned long > sectorHandler::getHighestLocalSector(){
    return resultHandler::highestSector();
}

//get the highest confirmed Sector of a Peer
tuple<vector<unsigned long long int>, unsigned long, unsigned long> sectorHandler::getHighestPeerSector(string ipAddress) {
    Message::MessageData data = std::array<char,1024>();
    //char* chr = ipAddress.data();
    const char *cstr1 = "FIND";
    const char *cstr2 = ipAddress.c_str();
    data.fill(*cstr1);
    data.fill(*cstr2);
    auto getMessage = Message(data);
    IOInterface ioInterface;
    ioInterface.queueOutgoingMessage(getMessage);

}

//get specific Result from another Peer
vector<unsigned long long > sectorHandler::findResultPeer(const string& ipAddress,unsigned long long sectorId){
    Message::MessageData data = std::array<char,1024>();
    //char* chr = ipAddress.data();

    const char *cstr1 = "GET";
    const char *cstr2 = ipAddress.c_str();
    data.fill(*cstr1);
    data.fill(*cstr2);
    auto getMessage = Message(data);
    IOInterface ioInterface;
    ioInterface.queueOutgoingMessage(getMessage);

}






