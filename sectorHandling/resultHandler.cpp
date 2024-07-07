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
map<unsigned long long , tuple<vector<unsigned long long>,unsigned long long, unsigned long long , int>> unconfirmedResultsOrderedMap;
// result vector | lowerBound | upperBound
tuple<vector<unsigned long long>,unsigned long long, unsigned long long > tupleConfirmed;
// initialize OrderedMap to save final Results
map<unsigned long long , tuple<vector<unsigned long long>,unsigned long long, unsigned long long >> confirmedResultsOrderedMap;

void resultHandler::initialize() {

    vector<unsigned long long> initVectorUnconfirmed = primeCalculation::calculatePrimes(1000, 2000);
    tupleUnconfirmed = make_tuple(initVectorUnconfirmed, 1000, 2000, 0);

    unconfirmedResultsOrderedMap[1] = tupleUnconfirmed;

    vector<unsigned long long> initVectorUnconfirmed2 = primeCalculation::calculatePrimes(2000, 3000);
    tupleUnconfirmed = make_tuple(initVectorUnconfirmed, 2000,3000, 2);
    unconfirmedResultsOrderedMap[2] = tupleUnconfirmed;

    vector<unsigned long long> initVectorConfirmed = primeCalculation::calculatePrimes(0, 1000);
    tupleConfirmed = make_tuple(initVectorConfirmed, 0, 1000);
    confirmedResultsOrderedMap[0] = tupleConfirmed;

}


void resultHandler::saveResultLocally(const vector<unsigned long long >& uncheckedVector,
                               unsigned long long lowerBound,
                               unsigned long long upperBound
) {
    //check if result is in confirmed results
    tuple<vector<unsigned long long int>,
            unsigned long long int,
            unsigned long long int>
            resultTupleLocally = findConfirmedResultLocally(lowerBound);
    vector<unsigned long long> resultConfirmedVector = get<0>(resultTupleLocally);

    if (resultConfirmedVector.at(0) == 0) {

        //check if a value is present in unconfirmed results
        tuple<vector<unsigned long long int>,
                unsigned long long int,
                unsigned long long int,
                int>
                resultTupleLocallyUnconfirmed = findUnconfirmedResultLocally(lowerBound);

        vector<unsigned long long> resultUnconfirmedLocally = get<0>(resultTupleLocallyUnconfirmed);
        if (resultUnconfirmedLocally.at(0) != 0) {

            //check if result is correct
            if(checkResult(uncheckedVector,lowerBound,upperBound)){
                int numberReceived = get<3>(resultTupleLocallyUnconfirmed);
                numberReceived = numberReceived + 1;
                if (numberReceived > 2){
                    resultTupleLocally = make_tuple(uncheckedVector,lowerBound,upperBound);
                    unsigned long long test = lowerBound/1000;
                    confirmedResultsOrderedMap[test] = resultTupleLocally;
                    unconfirmedResultsOrderedMap.erase(test);
                } else {
                resultTupleLocallyUnconfirmed = make_tuple(uncheckedVector,lowerBound,upperBound,numberReceived);
                unsigned long long test = lowerBound/1000;
                unconfirmedResultsOrderedMap[test] = resultTupleLocallyUnconfirmed;

                }
            }
        }

        if (resultUnconfirmedLocally.at(0) == 0) {
            if(checkResult(uncheckedVector,lowerBound,upperBound)){
                resultTupleLocallyUnconfirmed = make_tuple(uncheckedVector,lowerBound,upperBound,1);
                unsigned long long test = lowerBound/1000;
                unconfirmedResultsOrderedMap[test] = resultTupleLocallyUnconfirmed;

            }
        }
    }
}



// if returned tuple vector has 0 as elements result was not present
tuple<vector<unsigned long long int>,
        unsigned long long,
        unsigned long long > resultHandler::findConfirmedResultLocally(unsigned long long int sectorId) {

    tuple<vector<unsigned long long>,
            unsigned long long,
            unsigned long long >
            returnTuple;
    // init vector with zero values to distinguish result found from not found
    vector<unsigned long long > zeroVector;
    zeroVector.push_back(0);
    returnTuple = make_tuple(zeroVector,0,0);
    for (const auto & [key, value] : confirmedResultsOrderedMap){
        vector <unsigned long long> neededResult = get<0>(value);
        unsigned long long lowerSectorBound = get<1>(value);
        unsigned long long upperSectorBound = get<2>(value);
        if(sectorId == lowerSectorBound) {
            returnTuple = make_tuple(neededResult, lowerSectorBound, upperSectorBound);
        }
    }
    return returnTuple;
}


// if returned tuple vector has 0 as elements result was not found
tuple<vector<unsigned long long int>,
        unsigned long long,
        unsigned long long,
        int> resultHandler::findUnconfirmedResultLocally(unsigned long long int sectorId) {

    tuple<vector<unsigned long long>,
            unsigned long long,
            unsigned long long,
            int>
            returnTuple;
    // init vector with zero values to distinguish result found from not found
    vector<unsigned long long> zeroVector;
    zeroVector.push_back(0);
    returnTuple = make_tuple(zeroVector, 0, 0, 0);

    for (const auto & [key, value] : unconfirmedResultsOrderedMap){

        vector <unsigned long long> neededResult = get<0>(value);
        unsigned long long lowerSectorBound = get<1>(value);
        unsigned long long upperSectorBound = get<2>(value);
        int numberReceived = get<3>(value);

        if (sectorId == lowerSectorBound) {

            returnTuple = make_tuple(neededResult, lowerSectorBound, upperSectorBound,numberReceived);
        }
    }
    return returnTuple;
}

bool resultHandler::checkResult( vector<unsigned long long int> sectorResult,
                                 unsigned long long int lowerBound,
                                 unsigned long long int upperBound){
    bool sectorCorrect = true;
    vector<unsigned long long > ownCalculation = primeCalculation::calculatePrimes(lowerBound, upperBound);

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

bool resultHandler::compareResult(vector<unsigned long long int> presentVector, vector<unsigned long long int> uncheckedVector){
    for (int i = 0; i < presentVector.size(); ++i) {
        if (presentVector[i] != uncheckedVector[i]){
            return false;
        }
    }
    return true;
}


void resultHandler::printStuff(){
    cout << "Confirmed\n";
    for (const auto & [key, value] : confirmedResultsOrderedMap){
        vector <unsigned long long> neededResult = get<0>(value);
        unsigned long long lowerSectorBound = get<1>(value);
        unsigned long long upperSectorBound = get<2>(value);
        for (int j = 0; j < neededResult.size(); ++j) {
            //cout << neededResult[j] << " ";
        }
        cout << "Sector: "<< lowerSectorBound << " - " << upperSectorBound << "\n";

    }

    cout << "Unconfirmed\n";
    for (const auto & [key, value] : unconfirmedResultsOrderedMap){
        vector <unsigned long long> neededResult = get<0>(value);
        unsigned long long lowerSectorBound = get<1>(value);
        unsigned long long upperSectorBound = get<2>(value);
        int numberReceived = get<3>(value);

        for (int j = 0; j < neededResult.size(); ++j) {
            //cout << neededResult[j] << " ";
        }

        cout << "Sector: "<< lowerSectorBound << " - " << upperSectorBound << "\n";
        cout << "numberReceived: "<< numberReceived << "\n";

    }

    cout << "Round Finished\n\n";
}







