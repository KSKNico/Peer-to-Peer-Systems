#pragma once

#include "fingerTable.hpp"
#include "hash.hpp"
#include "globalDefinitions.hpp"

#include <map>
#include <vector>
#include <optional>
#include <cstdint>

// stores results that the peer is responsible for
// this is different from the resultHandler, which calculates the results and stores them
// temporarily until they are either sent to another peer or permanently stored in the resultStorage
class ResultStorage {
    public:
    ResultStorage() = delete;

    ResultStorage(FingerTable &fingerTable);

    // adds the results for the given lowerBound
    void addResults(uint64_t lowerBound, resultContainer results);

    // returns the results for the given lowerBound
    std::optional<resultContainer> getResults(uint64_t lowerBound);

    // returns true if the given lowerBound has results
    bool hasResults(uint64_t lowerBound);

    bool isResponsibleForResult(uint64_t lowerBound);

    // removes all results from the storage that the peer is no longer responsible for
    // and returns them in a map
    resultMap removeResults();

    private:
    FingerTable &fingerTable;

    resultMap allResults;
};