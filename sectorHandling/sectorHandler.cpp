
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
    //resultHandler::printStuff();
    vector<unsigned long long> testVector1 = primeCalculation::calculatePrimes(1000,2000);
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"192.168.178.1:12345");
    //resultHandler::printStuff();
    vector<unsigned long long> testVector2 = primeCalculation::calculatePrimes(2000,3000);
    sectorHandler::handleSectorResultCalculated(testVector2,2000,3000,"192.168.178.1:12345");
    //resultHandler::printStuff();
    vector<unsigned long long> testVector3 = primeCalculation::calculatePrimes(9000,10000);
    sectorHandler::handleSectorResultCalculated(testVector3,9000,10000,"192.168.178.1:12345");
    //resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"192.168.178.1:12345");
    //resultHandler::printStuff();
    sectorHandler::handleSectorResultCalculated(testVector1,1000,2000,"192.168.178.1:12345");
    //resultHandler::printStuff();

    //cout << "\n\n";
    //*/
}
bool sectorHandler::calculateNewSector(){
    unsigned long long lowerBound = primeCalculation::getLowerBound();
    unsigned long long upperBound = primeCalculation::getUpperBound();

    vector<unsigned long long> newCalc = primeCalculation::calculatePrimes(lowerBound,upperBound);
    sectorHandler::handleSectorResultCalculated(newCalc,lowerBound,upperBound,"-1");
    if (get<0>(resultHandler::findConfirmedResultLocally(lowerBound)).at(0) != 0){
        cout << "New Sector calculated\n";
        return true;
    }else if (get<0>(resultHandler::findUnconfirmedResultLocally(lowerBound)).at(0) != 0){
        cout << "New Sector calculated\n";
        return true;
    }
    return false;
}

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
        for (unsigned long long i : sectorResult) {
            vectorString.append(to_string(i));
            vectorString.append(delim);
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
        //cout << "putMessage queued\n";
    }



}

//find Result another Peer asks for locally
//setting lowerBound to -1 indicates search for highest local sector
tuple<vector<unsigned long long >,unsigned long long, unsigned long long > sectorHandler::findResultLocally(unsigned long long lowerBound){
    if (lowerBound == ULLONG_MAX){
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
    string s2 = "127.0.0.1:5004"+delim;
    string vectorString;
    vector<unsigned long long > vectorTest = primeCalculation::calculatePrimes(10000,11000);
    for (unsigned long long i : vectorTest) {
        vectorString.append(to_string(i));
        vectorString.append(",");
    }
    string s3 = vectorString;
    string s4 = s1+s2+s3;
    for (int i = 0; i < s4.size(); ++i) {
        data[i] = s4.at(i);
    }
    auto putMessage = Message(data);

    future<void> test5 = async(launch::async,sectorHandler::handleSectorResultFromPeer,putMessage);
    cout<<"\n";

}






