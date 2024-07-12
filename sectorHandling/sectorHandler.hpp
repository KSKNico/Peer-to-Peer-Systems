
#ifndef PRIME_FINDER_SECTORHANDLER_HPP
#define PRIME_FINDER_SECTORHANDLER_HPP

#include <unordered_map>
#include "../message.hpp"

using namespace std;

class sectorHandler {
public:
    static unordered_map<unsigned long long, vector<unsigned long long>> getAllResults();

    static void handleSectorResultFromPeer(Message message);

    static void handleSectorResultCalculated(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound,
            string ipAddress
    );

    static tuple<vector<unsigned long long int>, unsigned long long, unsigned long long>
    findResultLocally(unsigned long long int sectorId);

    static tuple<vector<unsigned long long>, unsigned long, unsigned long> getHighestLocalSector();

    //static tuple<vector<unsigned long long>,unsigned long, unsigned long >
    static void getHighestPeerSector(string ipAddress);

    static void initialize();

    static bool calculateNewSector(int lower_bound);

    static void findResultPeer(const string &ipAddress, unsigned long long int sectorId);

    static void testAll();
};


#endif //PRIME_FINDER_SECTORHANDLER_HPP
