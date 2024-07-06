
#include <iostream>
#include <list>
#include <vector>
#include <future>
#include "sectorHandler.hpp"
#include "primeCalculation.hpp"

using namespace std;

void sectorHandler::handleSectorResult(vector<unsigned long long> sectorResult, unsigned long long lowerBound, unsigned long long upperBound, int sectorId){
    /*TODO
     * call checkCalc
     *      1 if wrong throw away
     *      2 if right save and increase rightResultCount
     *          2.1 if threshold of 3 reached mark result as confirmed
     *              reevaluate highest known sectorResult
     *          2.2 if threshold of 3 not reached continue
     *      3
     */
     }

bool sectorHandler::checkCalculation(vector<unsigned long long> sectorResult, unsigned long long lowerBound, unsigned long long upperBound, int sectorId){

    //TODO search if SectorResults already exist before calculating again
    vector<unsigned long long > ownCalculation = primeCalculation::calculatePrimes(lowerBound, upperBound);
    bool correctSector = true;
    if (ownCalculation.size() != sectorResult.size()){
        correctSector = false;
        cout << "Wrong number of results";
        goto endloop;
    }
    for (int i = 0; i < ownCalculation.size(); ++i) {
        if (ownCalculation[i] != sectorResult[i]){
            cout << "Value at position " << i << " is " << sectorResult[i] << " should be " << ownCalculation[i];
            //TODO save Results as wrong? or throw away?
            correctSector = false;
            break;
        }
    }
    endloop:
    if (correctSector){
        //TODO increase number of correct results
        cout << "Sector correct";
        return correctSector;
    }
}

void sectorHandler::saveResult(const vector<unsigned long long >& result, int sectorId){
    //TODO
    int test = result.size();
    cout << test;
}
