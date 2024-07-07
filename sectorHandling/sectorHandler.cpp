
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
    /*TODO
     * call checkCalc
     *      1 if wrong throw away
     *      2 if right save and increase rightResultCount
     *          2.1 if threshold of 3 reached mark result as confirmed
     *              reevaluate highest known sectorResult
     *          2.2 if threshold of 3 not reached continue
     *      3
     */

    //check if already saved locally
    findResultLocally(lowerBound);
    //if result found in confirmed do nothing
    //if result found in unconfirmed change counter
    //if result neither confirmed nor unconfirmed check if right
    checkResult(sectorResult,lowerBound,upperBound);
    //if right save result and change counter | if wrong do nothing
    //resultHandler::saveResult(sectorResult);

     }

     //handles own outgoing calculation to responsible Peer
void sectorHandler::handleSectorResultCalculated(vector<unsigned long long> sectorResult,
                                                 unsigned long long lowerBound,
                                                 unsigned long long upperBound){
    //TODO Find Peer to Send result to

    resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
}

     //find Result another Peer asks for locally
tuple<vector<unsigned long long >,unsigned long long, unsigned long long > sectorHandler::findResultLocally(unsigned long long lowerBound){
         return resultHandler::findResultLocally(lowerBound);
}

    //get specific Result from another Peer
vector<unsigned long long > sectorHandler::findResultPeer(unsigned long long sectorId){

}

    //find the highest Sector you have confirmed results for
vector<unsigned long long> sectorHandler::getHighestLocalSector(){
    //resultHandler::
}
    //get the highest Sector of a Peer
vector<unsigned long long > sectorHandler::getHighestPeerSector() {

}

    //checks if result of another peer is correct
bool sectorHandler::checkResult(vector<unsigned long long> sectorResult, unsigned long long lowerBound, unsigned long long upperBound){
    return resultHandler::checkResult(sectorResult,lowerBound,upperBound);
}


