
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
    sectorHandler::handleSectorResultCalculated(testVector0,0,1000,"192.168.178.1:12345");
    resultHandler::printStuff();
    vector<unsigned long long> testVector1 = primeCalculation::calculatePrimes(1000,2000);
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"192.168.178.1:12345");
    resultHandler::printStuff();
    vector<unsigned long long> testVector2 = primeCalculation::calculatePrimes(2000,3000);
    sectorHandler::handleSectorResultCalculated(testVector2,2000,3000,"192.168.178.1:12345");
    resultHandler::printStuff();
    vector<unsigned long long> testVector3 = primeCalculation::calculatePrimes(9000,10000);
    sectorHandler::handleSectorResultCalculated(testVector3,9000,10000,"192.168.178.1:12345");
    resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"192.168.178.1:12345");
    resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"192.168.178.1:12345");
    resultHandler::printStuff();

    cout << "\n\n";
    //*/
}
future<void> sectorHandler::calculateNewSector(){
    unsigned long long lowerBound = primeCalculation::getLowerBound();
    unsigned long long upperBound = primeCalculation::getUpperBound();

    vector<unsigned long long> newCalc = primeCalculation::calculatePrimes(lowerBound,upperBound);
    sectorHandler::handleSectorResultCalculated(newCalc,lowerBound,upperBound,"-1");

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
                                                 unsigned long long upperBound,
                                                 string ipAddressPeer){
    if (ipAddressPeer == "-1"){
        resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
    } else{
        resultHandler::saveResultLocally(sectorResult,lowerBound,upperBound);
        Message::MessageData data = std::array<char,1024>();

        string delim = ",";
        string s1 = "PUT"+delim;
        string s2 = ipAddressPeer+delim;
        string vectorString;
        for (unsigned long long i : sectorResult) {
            vectorString.append(to_string(i));
            vectorString.append(delim);
        }
        string s3 = vectorString;
        string s4 = s1+s2+s3;
        for (int i = 0; i < s4.size(); ++i) {
            data[i] = s4.at(i);
        }
        for (const char& test : data) {
            cout << test;
        }
        auto putMessage = Message(data);
        IOInterface ioInterface;
        ioInterface.queueOutgoingMessage(putMessage);
    }



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
void sectorHandler::getHighestPeerSector(string ipAddress) {
    Message::MessageData data = std::array<char,1024>();
    //char* chr = ipAddress.data();
    string str1 = "GET,";
    string str2 = std::move(ipAddress)+",";
    string str3 = to_string(get<1>(sectorHandler::getHighestLocalSector()));
    string str4 = str1+str2+str3;
    cout << "\n" << str4 << "\n";
    for (int i = 0; i < str4.size(); ++i) {
        data[i] = str4.at(i);
    }

    auto getMessage = Message(data);
    IOInterface ioInterface;
    ioInterface.queueOutgoingMessage(getMessage);
}

//get specific Result from another Peer
vector<unsigned long long > sectorHandler::findResultPeer(const string& ipAddress,unsigned long long sectorId){
    Message::MessageData data = std::array<char,1024>();
    //char* chr = ipAddress.data();

    string str1 = "GET,";
    string str2 = ipAddress;
    string str3 = to_string(sectorId);
    string str4 = str1+str2+str3;
    for (int i = 0; i < str4.size(); ++i) {
        data[i] = str4.at(i);
    }

    auto getMessage = Message(data);
    IOInterface ioInterface;
    ioInterface.queueOutgoingMessage(getMessage);
}






