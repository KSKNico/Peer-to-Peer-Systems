
#include <iostream>
#include <list>
#include <vector>
#include <future>
#include <utility>
#include "primeCalculation.hpp"

using namespace std;

    unsigned long long primeCalculation::getLowerBound(){
        //TODO
        unsigned long long lowerBound = 0;
        return lowerBound;
}
    unsigned long long primeCalculation::getUpperBound(){
        //TODO

        unsigned long long upperBound = 1000;
        return upperBound;
}



vector<unsigned long long > primeCalculation::calculatePrimes(unsigned long long lowerBound, unsigned long long upperBound){
        vector<unsigned long long > primes;
        for(unsigned long long i=lowerBound; i <= upperBound; i++)
        {
            bool isPrime=true;
            if (i == 0 || i == 1){
                isPrime= false;
            }
            for(unsigned long long j=2;j<i;j++)
            {
                if(i % j == 0)
                {
                    //cout <<"not prime "<< i << "\n";
                    isPrime=false;
                    break;
                }
            }
            if(isPrime)
            {
                primes.push_back(i);
                cout << "prime "<< i << " | ";
            }
        }
        return primes;
}

void primeCalculation::calculateCurrentSector() {
        unsigned long long  lowerBound = getLowerBound();
        unsigned long long  upperBound = getUpperBound();

        //vector<int > test3 = primeCalculation::calculatePrimes(test,test2);
        cout << "Test1 \n";
        std::future<vector<unsigned long long>> currentSectorPrimes = std::async (primeCalculation::calculatePrimes,lowerBound,upperBound);
        cout << "Test2 \n";
        vector<unsigned long long >  currentSectorPrimeResult = currentSectorPrimes.get();
        cout << "\n" << "Test3 \n";

}

