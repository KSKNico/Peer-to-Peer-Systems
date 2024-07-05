
#ifndef PRIME_FINDER_PRIME_CALCULATION_HPP
#define PRIME_FINDER_PRIME_CALCULATION_HPP

using namespace std;

class primeCalculation {




public: static void calculateCurrentSector();

    static unsigned long long getLowerBound();

    static  unsigned long long getUpperBound();

    public: static vector<unsigned long long >calculatePrimes(unsigned long long  lowerBound, unsigned long long upperBound);

    public:
    static void checkSectorResult(const vector<unsigned long long int>& sectorResult, unsigned long long int lowerBound,
                                  unsigned long long int upperBound);
};



#endif //PRIME_FINDER_PRIME_CALCULATION_HPP