
#ifndef PRIME_FINDER_SECTORHANDLER_HPP
#define PRIME_FINDER_SECTORHANDLER_HPP
#include "../message.hpp"

using namespace std;

class sectorHandler {

public:
    static void handleSectorResultFromPeer(Message message);

public:
    static void handleSectorResultCalculated(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound
            );

public:
    static tuple<vector<unsigned long long int>,
            unsigned long long,
            unsigned long long >
            findResultLocally(unsigned long long int sectorId);

    tuple<vector<unsigned long long>,unsigned long, unsigned long > getHighestLocalSector();

    tuple<vector<unsigned long long>,unsigned long, unsigned long > getHighestPeerSector(string ipAddress);

    static void initialize();

    static future<void> calculateNewSector();

    static vector<unsigned long long int> findResultPeer(const string& ipAddress, unsigned long long int sectorId);
};


#endif //PRIME_FINDER_SECTORHANDLER_HPP
