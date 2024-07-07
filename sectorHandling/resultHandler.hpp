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
    static void saveResultLocally(const vector<unsigned long long int> &result,
                                  unsigned long long lowerBound,
                                  unsigned long long upperBound);

public:
    static tuple<vector<unsigned long long int>,unsigned long long, unsigned long long > findResultLocally (unsigned long long sectorId);

public:
    static bool checkResult(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound
    );

};


#endif //PRIME_FINDER_RESULTHANDLER_HPP
