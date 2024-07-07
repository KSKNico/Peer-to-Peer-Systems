//
// Created by tobi on 07.07.24.
//

#include <iostream>
#include <list>
#include <vector>
#include <future>
#include <map>
#include "resultHandler.hpp"
#include "primeCalculation.hpp"

using namespace std;

// result vector | lowerBound | upperBound | how often result was received
tuple<vector<unsigned long long>,unsigned long long, unsigned long long , int> tupleUnconfirmed;
// initialize OrderedMap to save preliminary Results
map<int, tuple<vector<unsigned long long>,unsigned long long, unsigned long long , int>> unconfirmedResultsOrderedMap;
// result vector | lowerBound | upperBound
tuple<vector<unsigned long long>,unsigned long long, unsigned long long > tupleConfirmed;
// initialize OrderedMap to save final Results
map<int, tuple<vector<unsigned long long>,unsigned long long, unsigned long long >> confirmedResultsOrderedMap;

void resultHandler::initialize() {

    vector<unsigned long long> initVectorUnconfirmed = primeCalculation::calculatePrimes(1000, 2000);
    tupleUnconfirmed = make_tuple(initVectorUnconfirmed, 1000, 2000, 0);
    unconfirmedResultsOrderedMap[0] = tupleUnconfirmed;

    vector<unsigned long long> initVectorUnconfirmed2 = primeCalculation::calculatePrimes(3000, 4000);
    tupleUnconfirmed = make_tuple(initVectorUnconfirmed, 3000,4000, 1);
    unconfirmedResultsOrderedMap[0] = tupleUnconfirmed;

    vector<unsigned long long> initVectorConfirmed = primeCalculation::calculatePrimes(0, 1000);
    tupleConfirmed = make_tuple(initVectorConfirmed, 0, 1000);
    confirmedResultsOrderedMap[0] = tupleConfirmed;

    vector<unsigned long long> initVectorConfirmed2 = primeCalculation::calculatePrimes(2000, 3000);
    tupleConfirmed = make_tuple(initVectorConfirmed, 2000, 3000);
    confirmedResultsOrderedMap[1] = tupleConfirmed;
}


void resultHandler::saveResultLocally(const vector<unsigned long long >& result,
                               unsigned long long lowerBound,
                               unsigned long long upperBound
){
    tuple<vector<unsigned long long int>,
            unsigned long long int,
            unsigned long long int>
            resultLocally = findResultLocally(lowerBound);
    unsigned long long test = get<0>(resultLocally).at(0);
    if(get<0>(resultLocally).at(0) == 0){
        cout << "Result not present yet";
    } else {
        cout << "Result is there:" << test;
    }

}


bool resultHandler::checkResult( vector<unsigned long long int> sectorResult,
                                 unsigned long long int lowerBound,
                                 unsigned long long int upperBound){
    vector<unsigned long long > ownCalculation = primeCalculation::calculatePrimes(lowerBound, upperBound);
    bool sectorCorrect = true;
    if (ownCalculation.size() != sectorResult.size()){
        sectorCorrect = false;
        cout << "Wrong number of results";
        goto endloop;
    }
    for (int i = 0; i < ownCalculation.size(); ++i) {
        if (ownCalculation[i] != sectorResult[i]){
            cout << "Value at position " << i << " is " << sectorResult[i] << " should be " << ownCalculation[i];
            //TODO save Results as wrong? or throw away?
            sectorCorrect = false;
            break;
        }
    }
    endloop:
    return sectorCorrect;
}


tuple<vector<unsigned long long int>,
        unsigned long long,
        unsigned long long > resultHandler::findResultLocally(unsigned long long int sectorId) {
    tuple<vector<unsigned long long>,
            unsigned long long,
            unsigned long long >
            returnTuple;
    vector<unsigned long long > zeroVector;
    zeroVector.push_back(0);
    zeroVector.push_back(0);
    zeroVector.push_back(0);
    returnTuple = make_tuple(zeroVector,0,0);
    cout << get<0>(returnTuple).at(0);
    for (int i = 0; i <confirmedResultsOrderedMap.size(); ++i) {

        vector <unsigned long long> neededResult = get<0>(confirmedResultsOrderedMap.at(i));
        unsigned long long lowerSectorBound = get<1>(confirmedResultsOrderedMap.at(i));
        unsigned long long upperSectorBound = get<2>(confirmedResultsOrderedMap.at(i));
        if(sectorId == lowerSectorBound) {
            //cout << "Found Sector in confirmed Results" << "\n";
            for (unsigned long long j = 0; j < neededResult.size(); ++j) {
                cout << neededResult[j] << " | ";
            }
            //cout << "\n" << "lB is " << lowerSectorBound << "\n" << "uB is " << upperSectorBound << "\n";

            returnTuple = make_tuple(neededResult, lowerSectorBound, upperSectorBound);

        }
    }

    return returnTuple;
}






