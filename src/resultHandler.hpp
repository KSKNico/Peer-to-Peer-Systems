#pragma once

#include <map>
#include <vector>
#include <mutex>
#include <future>
#include <optional>

#include "globalDefinitions.hpp"




// stores the results that the peer has calculated
class ResultHandler {
    public:

    using ResultFuture = std::future<std::vector<uint64_t>>;

    // returns the highest lowerBound for which results are available
    // meaning that the results for lowerBound + INTERVAL_SIZE are not yet available
    // uint64_t getHighest();

    // adds the results for the given lowerBound
    void addResults(uint64_t lowerBound, std::vector<uint64_t> results);

    // returns the results for the given lowerBound
    std::optional<std::vector<uint64_t>> getResults(uint64_t lowerBound);

    // calculates the results for the given lowerBound
    void submitCalculation(uint64_t lowerBound);

    // returns true if the given lowerBound has results
    bool hasResults(uint64_t lowerBound);



    // returns true if the given lowerBound is actively calculated
    bool isActivelyCalculated(uint64_t lowerBound);

    private:
    // calculates the prime numbers between the given lowerBound and lowerBound + INTERVAL_SIZE
    // i. e. the interval [lowerBound, lowerBound + INTERVAL_SIZE)
    // should not be called directly, use submitCalculation instead
    static std::vector<uint64_t> calculatePrimes(uint64_t lowerBound);

    // updates the results with the completed futures and removes them from the futures map
    void updateResultsWithFutures();

    // updates the results with a single future if it is completed
    void updateResultWithSingleFuture(uint64_t lowerBound);

    std::map<uint64_t, ResultFuture> futures;
    std::mutex resultsMutex;
    std::mutex futuresMutex;
    std::map<uint64_t, std::vector<uint64_t>> results;
};