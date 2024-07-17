#pragma once

#include <map>
#include <vector>
#include <mutex>
#include <future>
#include <optional>

using ull = unsigned long long;
constexpr ull INTERVAL_SIZE = 1000;

class ResultHandler {
    public:

    using ResultFuture = std::future<std::vector<ull>>;

    // returns the highest lowerBound for which results are available
    // meaning that the results for lowerBound + INTERVAL_SIZE are not yet available
    ull getHighest();

    // adds the results for the given lowerBound
    void addResults(ull lowerBound, std::vector<ull> results);

    // returns the results for the given lowerBound
    std::optional<std::vector<ull>> getResults(ull lowerBound);

    // calculates the results for the given lowerBound
    void submitCalculation(ull lowerBound);

    // returns true if the given lowerBound has results
    bool hasResults(ull lowerBound);



    // returns true if the given lowerBound is actively calculated
    bool isActivelyCalculated(ull lowerBound);

    private:
    // calculates the prime numbers between the given lowerBound and lowerBound + INTERVAL_SIZE
    // should not be called directly, use submitCalculation instead
    static std::vector<ull> calculatePrimes(ull lowerBound);

    // updates the results with the completed futures and removes them from the futures map
    void updateResultsWithFutures();

    // updates the results with a single future if it is completed
    void updateResultWithSingleFuture(ull lowerBound);

    std::map<ull, ResultFuture> futures;
    std::mutex resultsMutex;
    std::mutex futuresMutex;
    std::map<ull, std::vector<ull>> results;
};