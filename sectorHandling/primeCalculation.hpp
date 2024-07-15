
#ifndef PRIME_FINDER_PRIME_CALCULATION_HPP
#define PRIME_FINDER_PRIME_CALCULATION_HPP

using namespace std;

class primeCalculation {

public:
    static std::future<vector<unsigned long long >> calculateCurrentSector();

    static unsigned long long getLowerBound();

    static  unsigned long long getUpperBound();

    public: static vector<unsigned long long >calculatePrimes(unsigned long long  lowerBound);


};



#endif //PRIME_FINDER_PRIME_CALCULATION_HPP
