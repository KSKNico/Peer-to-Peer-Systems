
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
#include "../iointerface.hpp"
#include "../peer.hpp"
using namespace std;

void sectorHandler::initialize(){
    //initialize map with confirmed results and unconfirmed results
    resultHandler::initialize();
    ///* for testing purposes
    vector<unsigned long long> testVector0 = primeCalculation::calculatePrimes(0,1000);
    sectorHandler::handleSectorResultCalculated(testVector0,0,1000,"-1");
    //resultHandler::printStuff();
    vector<unsigned long long> testVector1 = primeCalculation::calculatePrimes(1000,2000);
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"-1");
    //resultHandler::printStuff();
    vector<unsigned long long> testVector2 = primeCalculation::calculatePrimes(2000,3000);
    sectorHandler::handleSectorResultCalculated(testVector2,2000,3000,"-1");
    //resultHandler::printStuff();
    vector<unsigned long long> testVector3 = primeCalculation::calculatePrimes(9000,10000);
    sectorHandler::handleSectorResultCalculated(testVector3,9000,10000,"-1");
    //resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"-1");
    //resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"-1");
    //resultHandler::printStuff();

    //cout << "\n\n";
    //*/
}
bool sectorHandler::calculateNewSector(int lower_bound){
    /*unsigned long long lowerBound = primeCalculation::getLowerBound();
    unsigned long long upperBound = primeCalculation::getUpperBound();*/
    int upper_bound = lower_bound + 1000;

    vector<unsigned long long> newCalc = primeCalculation::calculatePrimes(lower_bound,upper_bound);
    sectorHandler::handleSectorResultCalculated(newCalc,lower_bound,upper_bound,"-1");
    if (get<0>(resultHandler::findConfirmedResultLocally(lower_bound)).at(0) != 0){
        //pair<unsigned long long, vector<unsigned long long >> newCalcPair (lowerBound,newCalc);
        //Peer::prime_intervals.insert(newCalcPair);
        cout << "New Sector calculated\n";
        return true;
    }else if (get<0>(resultHandler::findUnconfirmedResultLocally(lower_bound)).at(0) != 0){
        cout << "New Sector calculated\n";
        return true;
    }
    return false;
}
//pair<unsigned long long, vector<unsigned long long >> newCalcPair (lowerBound,newCalc);
//Peer::prime_intervals.insert(newCalcPair);



//handle incoming result from another peer
void sectorHandler::handleSectorResultFromPeer(Message message){
    cout<< "Result from Peer saved locally\n";
    Message::put_message decodePutMessage = message.decode_put_message();
    vector<unsigned long long> sectorResult = decodePutMessage.primes;
    unsigned long long lowerBound = decodePutMessage.start_of_interval;
    unsigned long long upperBound  = lowerBound+1000;
    resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
}

     //handles calculation we did ourselves
     //     setting ipAddress to -1 = result gets only saved locally
     //     else save locally and send to responsible Peer
void sectorHandler::handleSectorResultCalculated(vector<unsigned long long> sectorResult,
                                                 unsigned long long lowerBound,
                                                 unsigned long long upperBound,
                                                 string ipAddressPeer){
    string minusOne = to_string(-1);
    if (ipAddressPeer == minusOne){
        cout << "Local Result only\n";
        resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
    } else{
        resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
        Message::MessageData data = std::array<char,1024>();

        string delim = ",";
        string s1 = "PUT"+delim;
        string s2 = ipAddressPeer+delim;
        string vectorString;
        for (int i = 0 ; i < sectorResult.size();i++) {
            vectorString.append(to_string(sectorResult[i]));
            if ((i<sectorResult.size()-1))
                vectorString.append(",");
        }
        string s3 = vectorString;
        string s4 = s1+s2+s3;
        for (int i = 0; i < s4.size(); ++i) {
            data[i] = s4.at(i);
            //cout << data[i];
        }
        auto putMessage = Message(data);
        IOInterface ioInterface;
        ioInterface.queueOutgoingMessage(putMessage);
        cout << "putMessage queued for Peer\n";
    }



}

//find Result another Peer asks for locally
//setting lowerBound to -1 indicates search for highest local sector
tuple<vector<unsigned long long >,unsigned long long, unsigned long long > sectorHandler::findResultLocally(unsigned long long lowerBound){
    if (lowerBound == ULLONG_MAX){
        cout << "Returning highest sector";
        return sectorHandler::getHighestLocalSector();

    }
    cout<< "Local Result found\n";
    return resultHandler::findConfirmedResultLocally(lowerBound);
}

//find the highest Sector you have confirmed results for
tuple<vector<unsigned long long>,unsigned long, unsigned long > sectorHandler::getHighestLocalSector(){
    cout<<"Returning highest local Sector\n";
    return resultHandler::highestSector();
}

//get the highest confirmed Sector of a Peer
//lowerBound set to -1 to indicate search for highest available sector at peer
void sectorHandler::getHighestPeerSector(string ipAddress) {
    Message::MessageData data = std::array<char,1024>();
    string str1 = "GET,";
    string str2 = std::move(ipAddress)+",";
    string str3 = to_string(-1);
    string str4 = str1+str2+str3;
    for (int i = 0; i < str4.size(); ++i) {
        data[i] = str4.at(i);
        //cout << data[i];
    }

    auto getMessage = Message(data);
    IOInterface ioInterface;
    ioInterface.queueOutgoingMessage(getMessage);
    cout << "Queued Message for highest result from peer\n";
}

//get specific Result from another Peer
void sectorHandler::findResultPeer(const string& ipAddress,unsigned long long sectorId){
    Message::MessageData data = std::array<char,1024>();

    string str1 = "GET,";
    string str2 = ipAddress+",";
    string str3 = to_string(sectorId);
    string str4 = str1+str2+str3;
    for (int i = 0; i < str4.size(); ++i) {
        data[i] = str4.at(i);
        //cout << data[i];
    }

    auto getMessage = Message(data);
    IOInterface ioInterface;
    ioInterface.queueOutgoingMessage(getMessage);
    cout<<"findResultPeerMessage queued\n";
}

void sectorHandler::testAll(){

    future<bool> sectorCalculated = async(launch::async,sectorHandler::calculateNewSector);
    cout << "\n";
    future<void> test = async(launch::async,sectorHandler::getHighestPeerSector, "127.0.0.1:5001");
    cout << "\n";
    future<void> test2 = async(launch::async,sectorHandler::findResultPeer,"127.0.0.1:5002",0);
    cout << "\n";
    vector<unsigned long long > vector1 = (primeCalculation::calculatePrimes(0,1000));
    future<void> test3 = async(launch::async,sectorHandler::handleSectorResultCalculated,vector1,0,1000,"-1");
    cout << "\n";
    future<tuple<vector<unsigned long long >,unsigned long long, unsigned long long >> test4 = async(launch::async,sectorHandler::findResultLocally,0);
    cout << "\n";

    Message::MessageData data = std::array<char,1024>();
    string delim = ",";
    string s1 = "PUT"+delim;
    string s2 = "127.0.0.1:5002"+delim+"10000,";
    string vectorString;
    vector<unsigned long long > vectorTest = primeCalculation::calculatePrimes(10000,11000);
    for (int i = 0 ; i < vectorTest.size();i++) {
        vectorString.append(to_string(vectorTest[i]));
        if ((i<vectorTest.size()-1))
            vectorString.append(",");
    }
    string s3 = vectorString;
    string s4 = s1+s2+s3;
    for (int i = 0; i < s4.size(); ++i) {
        data[i] = s4.at(i);
    }
    auto putMessage = Message(data);

    //future<void> test5 = async(launch::async,sectorHandler::handleSectorResultFromPeer,putMessage);
    auto peer_addr_1 = Poco::Net::SocketAddress("127.0.0.1:5005");
    Peer peer_1(peer_addr_1);
    peer_1.process_put_message(putMessage);
    cout<<"\n";

}

unordered_map<unsigned long long, vector<unsigned long long>> sectorHandler::getAllResults() {

    map<unsigned long long, tuple<vector<unsigned long long>, unsigned long long, unsigned long long >>
            mapUnedited = resultHandler::getAllConfirmedResults();
    unordered_map<unsigned long long, vector<unsigned long long>> resultsEdited;

    for (int i = 0; i < mapUnedited.size(); ++i) {
        tuple<vector<unsigned long long>, unsigned long long, unsigned long long >tupleUnedited = mapUnedited[i];
        vector<unsigned long long> vectorUnedited = get<0>(tupleUnedited);
        unsigned long long lowerBound = get<1>(tupleUnedited);
        //cout << lowerBound << " " << vectorUnedited.at(i) << "\n";
        resultsEdited.insert(
                pair <unsigned long long,
                        vector<unsigned long long>>
                        (lowerBound, vectorUnedited));
    }
    return resultsEdited;
}







