
#ifndef PRIME_FINDER_SECTORHANDLER_HPP
#define PRIME_FINDER_SECTORHANDLER_HPP

#include <unordered_map>
#include "../message.hpp"
#include "Poco/Net/SocketAddress.h"

using namespace std;

class sectorHandler {
public:
    static unordered_map<unsigned long long, vector<unsigned long long>> getAllResults();

    static void handleSectorResultFromPeer(Message message);

    static optional<Message> handleSectorResultCalculated(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound,
            const Poco::Net::SocketAddress& ipAddress,
            const Poco::Net::SocketAddress& ownAddress
            );

    static tuple<vector<unsigned long long int>,
            unsigned long long,
            unsigned long long >
            findResultLocally(unsigned long long int sectorId);

    static tuple<vector<unsigned long long>,unsigned long, unsigned long > getHighestLocalSector();

    //static tuple<vector<unsigned long long>,unsigned long, unsigned long >
    static Message getHighestPeerSector(string ipAddress);

    static void initialize(const Poco::Net::SocketAddress& ownAddress);

    static bool calculateNewSector(unsigned long long lowerBound, const Poco::Net::SocketAddress& ownAddress);

    static Message findResultOtherPeer(const string& ipAddress, unsigned long long int sectorId);

    static void testAll();
};


#endif //PRIME_FINDER_SECTORHANDLER_HPP
