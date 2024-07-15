
#include <iostream>
#include <list>
#include <vector>
#include <future>
#include "primeCalculation.hpp"

using namespace std;

    unsigned long long primeCalculation::getLowerBound(){
        //TODO
        unsigned long long lowerBound;
        try {
//            future<unsigned long long> lowerBound = async(launch::async, );
            lowerBound = static_cast<unsigned long long int>(0);
        } catch (const exception e){
            cerr << e.what();
        }
        return lowerBound;
}
    unsigned long long primeCalculation::getUpperBound(){
        //TODO
        unsigned long long upperBound;
        try {
            //future<unsigned long long> lowerBound = async();
            upperBound = static_cast<unsigned long long int>(1000);

        } catch (const exception e){
            cerr << e.what();
        }return upperBound;
}



vector<unsigned long long > primeCalculation::calculatePrimes(unsigned long long lowerBound){
        unsigned long long upperBound = lowerBound+1000;
        std::cout << "i calculate " << lowerBound << " to " << upperBound << std::endl;
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
                //cout << "prime "<< i << " | ";
            }
        }
        return primes;
}

