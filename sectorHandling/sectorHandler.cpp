
#include <iostream>
#include <list>
#include <vector>
#include <future>
#include <map>
#include "sectorHandler.hpp"
#include "resultHandler.hpp"
#include "primeCalculation.hpp"

using namespace std;

//handle incoming result from another peer
void sectorHandler::handleSectorResultFromPeer(vector<unsigned long long> sectorResult,
                                               unsigned long long lowerBound,
                                               unsigned long long upperBound){
    resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);

     }

     //handles calculation we did ourselves
     //     save locally?
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
vector<unsigned long long> sectorHandler::getHighestLocalSector(){

}

//get the highest confirmed Sector of a Peer
vector<unsigned long long > sectorHandler::getHighestPeerSector() {
    //TODO
}

//get specific Result from another Peer
vector<unsigned long long > sectorHandler::findResultPeer(unsigned long long sectorId){
    //TODO
}




