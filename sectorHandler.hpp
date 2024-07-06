
#ifndef PRIME_FINDER_SECTORHANDLER_HPP
#define PRIME_FINDER_SECTORHANDLER_HPP

using namespace std;

class sectorHandler {
    public:
    static bool checkCalculation(
            vector<unsigned long long int> sectorResult,
            unsigned long long int lowerBound,
            unsigned long long int upperBound,
            int sectorId
            );

    void saveResult(const vector<unsigned long long int>& result, int sectorId);
};


#endif //PRIME_FINDER_SECTORHANDLER_HPP
