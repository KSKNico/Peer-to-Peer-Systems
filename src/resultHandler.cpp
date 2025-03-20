#include "resultHandler.hpp"

#include <iostream>

void ResultHandler::updateResultWithSingleFuture(uint64_t lowerBound) {
    std::unique_lock<std::mutex> lock(futuresMutex);
    auto it = futures.find(lowerBound);
    if (it == futures.end()) {
        return;
    }
    std::future_status status = it->second.wait_for(std::chrono::seconds(0));
    if (status == std::future_status::ready) {
        std::vector<uint64_t> results = it->second.get();
        addResults(lowerBound, results);
        futures.erase(it);
    }
}

void ResultHandler::updateResultsWithFutures() {
    std::unique_lock<std::mutex> lock(futuresMutex);
    std::vector<uint64_t> toErase;
    for (auto &futurePair : futures) {
        // get the status
        std::future_status status = futurePair.second.wait_for(std::chrono::seconds(0));
        if (status == std::future_status::ready) {
            // get the result
            std::vector<uint64_t> results = futurePair.second.get();
            addResults(futurePair.first, results);
            toErase.push_back(futurePair.first);
        } else if (status == std::future_status::timeout) {
            // do nothing
        } else if (status == std::future_status::deferred) {
            // do nothing
        }
    }
    for (uint64_t lowerBound : toErase) {
        futures.erase(lowerBound);
    }
}

bool ResultHandler::isActivelyCalculated(uint64_t lowerBound) {
    std::unique_lock<std::mutex> lock(futuresMutex);
    return futures.find(lowerBound) != futures.end();
}

/*
uint64_t ResultHandler::getHighest() {
    updateResultsWithFutures();
    std::unique_lock<std::mutex> lock(resultsMutex);
    if (results.empty()) {
        return 0;
    }
    return results.rbegin()->first;
}
*/

bool ResultHandler::hasResults(uint64_t lowerBound) {
    updateResultWithSingleFuture(lowerBound);
    std::unique_lock<std::mutex> lock(resultsMutex);
    return results.find(lowerBound) != results.end();
}

void ResultHandler::addResults(uint64_t lowerBound, std::vector<uint64_t> primeNumbers) {
    std::unique_lock<std::mutex> lock(resultsMutex);
    results[lowerBound] = primeNumbers;
}

std::optional<std::vector<uint64_t>> ResultHandler::getResults(uint64_t lowerBound) {
    updateResultWithSingleFuture(lowerBound);
    std::unique_lock<std::mutex> lock(resultsMutex);
    auto it = results.find(lowerBound);
    if (it == results.end()) {
        return std::nullopt;
    } else {
        return std::make_optional(it->second);
    }
}

void ResultHandler::submitCalculation(uint64_t lowerBound) {
    std::unique_lock<std::mutex> lock(futuresMutex);
    if (futures.find(lowerBound) != futures.end()) {
        return;
    }
    futures[lowerBound] = std::async(std::launch::async, &ResultHandler::calculatePrimes, lowerBound);
}

std::vector<uint64_t> ResultHandler::calculatePrimes(uint64_t lowerBound) {
    std::vector<uint64_t> primes;
    uint64_t upperBound = lowerBound + INTERVAL_SIZE;
    for (uint64_t i = lowerBound; i <= upperBound; i++) {
        bool isPrime = true;
        if (i == 0 || i == 1) {
            isPrime = false;
        }
        for (uint64_t j = 2; j < i; j++) {
            if (i % j == 0) {
                // cout <<"not prime "<< i << "\n";
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            primes.push_back(i);
            // cout << "prime "<< i << " | ";
        }
    }
    return primes;
}