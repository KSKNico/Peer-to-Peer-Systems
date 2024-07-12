#include "resultHandler.hpp"

void ResultHandler::updateResultWithSingleFuture(ull lowerBound) {
    std::unique_lock<std::mutex> lock(futuresMutex);
    auto it = futures.find(lowerBound);
    if (it == futures.end()) {
        return;
    }
    std::future_status status = it->second.wait_for(std::chrono::seconds(0));
    if (status == std::future_status::ready) {
        std::vector<ull> results = it->second.get();
        addResults(lowerBound, results);
        futures.erase(it);
    }
}

void ResultHandler::updateResultsWithFutures() {
    std::unique_lock<std::mutex> lock(futuresMutex);
    std::vector<ull> toErase;    
    for (auto &futurePair : futures) {
        // get the status 
        std::future_status status = futurePair.second.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready) {
            // get the result
            std::vector<ull> results = futurePair.second.get();
            addResults(futurePair.first, results);
            toErase.push_back(futurePair.first);
        } else if (status == std::future_status::timeout) {
            // do nothing
        } else if (status == std::future_status::deferred) {
            // do nothing
        }
    }
    for (ull lowerBound : toErase) {
        futures.erase(lowerBound);
    }
}

bool ResultHandler::isActivelyCalculated(ull lowerBound) {
    std::unique_lock<std::mutex> lock(futuresMutex);
    return futures.find(lowerBound) != futures.end();
}

ull ResultHandler::getHighest() {
    updateResultsWithFutures();
    std::unique_lock<std::mutex> lock(resultsMutex);
    if (results.empty()) {
        return 0;
    }
    return results.rbegin()->first;
}

bool ResultHandler::hasResults(ull lowerBound) {
    updateResultWithSingleFuture(lowerBound);
    std::unique_lock<std::mutex> lock(resultsMutex);
    return results.find(lowerBound) != results.end();
}

void ResultHandler::addResults(ull lowerBound, std::vector<ull> primeNumbers) {
    std::unique_lock<std::mutex> lock(resultsMutex);
    results[lowerBound] = primeNumbers;
}

std::optional<std::vector<ull>> ResultHandler::getResults(ull lowerBound) {
    updateResultWithSingleFuture(lowerBound);
    std::unique_lock<std::mutex> lock(resultsMutex);
    auto it = results.find(lowerBound);
    if (it == results.end()) {
        return std::nullopt; 
    } else {
        return std::make_optional(it->second); 
    }

}

void ResultHandler::submitCalculation(ull lowerBound) {
    std::unique_lock<std::mutex> lock(futuresMutex);
    if (futures.find(lowerBound) != futures.end()) {
        return;
    }
    futures[lowerBound] = std::async(std::launch::async, &ResultHandler::calculatePrimes, lowerBound);
}

std::vector<ull> ResultHandler::calculatePrimes(ull lowerBound) {
        std::vector<ull> primes;
        ull upperBound = lowerBound + INTERVAL_SIZE;
        for(ull i=lowerBound; i <= upperBound; i++)
        {
            bool isPrime=true;
            if (i == 0 || i == 1){
                isPrime= false;
            }
            for(ull j=2;j<i;j++)
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