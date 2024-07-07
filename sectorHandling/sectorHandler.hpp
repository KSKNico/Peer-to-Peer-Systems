
#ifndef PRIME_FINDER_SECTORHANDLER_HPP
#define PRIME_FINDER_SECTORHANDLER_HPP

using namespace std;

class sectorHandler {


    void handleSectorResultFromPeer(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound
            );

    void handleSectorResultCalculated(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound
            );

public:
    static tuple<vector<unsigned long long int>,
            unsigned long long,
            unsigned long long >
            findResultLocally(unsigned long long int sectorId);

    vector<unsigned long long int> findResultPeer(unsigned long long int lowerBound);

    vector<unsigned long long int> getHighestLocalSector();

    vector<unsigned long long int> getHighestPeerSector();

    static bool checkResult(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound
            );

};


#endif //PRIME_FINDER_SECTORHANDLER_HPP
