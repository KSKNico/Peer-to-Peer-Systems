//
// Created by tobi on 07.07.24.
//

#ifndef PRIME_FINDER_RESULTHANDLER_HPP
#define PRIME_FINDER_RESULTHANDLER_HPP

using namespace std;


class resultHandler {

public:
    static void initialize();

public:
    static map<unsigned long long , tuple<vector<unsigned long long>,unsigned long long, unsigned long long >>
    getAllConfirmedResults();

public:
    static void saveResultLocally(const vector<unsigned long long int> &uncheckedVector,
                                  unsigned long long lowerBound,
                                  unsigned long long upperBound);

public:
    static bool checkResult(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound
    );

public:
    static tuple<vector<unsigned long long int>,unsigned long long, unsigned long long > findConfirmedResultLocally (unsigned long long sectorId);

public:
    static tuple<
            vector<unsigned long long int>,
            unsigned long long int,
            unsigned long long int,
            int>
            findUnconfirmedResultLocally(unsigned long long int sectorId);

    static bool compareResult(vector<unsigned long long int> presentVector, vector<unsigned long long int> uncheckedVector);

    static void printStuff();

    static tuple<vector<unsigned long long int>, unsigned long long int, unsigned long> highestSector();
};


#endif //PRIME_FINDER_RESULTHANDLER_HPP
