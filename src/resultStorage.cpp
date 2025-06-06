#include "resultStorage.hpp"

#include <cassert>

ResultStorage::ResultStorage(FingerTable& fingerTable) : fingerTable(fingerTable) {}

bool ResultStorage::isResponsibleForResult(uint64_t lowerBound) {
    if (Hash(fingerTable.getPredecessor()) == fingerTable.getOwnHash()) {
        // this means no peer is before this peer
        // so this peer is responsible for the time being
        return true;
    }

    return Hash::hashInterval(lowerBound)
        .isBetween(Hash::hashSocketAddress(fingerTable.getPredecessor()), 
                   fingerTable.getOwnHash());
}

void ResultStorage::addResults(uint64_t lowerBound, resultContainer results) {
    assert(isResponsibleForResult(lowerBound));
    allResults[lowerBound] = results;
}

resultMap ResultStorage::removeResults() {
    resultMap results;
    for (auto it = allResults.begin(); it != allResults.end();) {
        if (!isResponsibleForResult(it->first)) {
            results[it->first] = it->second;
            it = allResults.erase(it);
        } else {
            ++it;
        }
    }
    return results;
}

bool ResultStorage::hasResults(uint64_t lowerBound) {
    return allResults.find(lowerBound) != allResults.end();
}

// returns the interval for which results are available
resultType ResultStorage::getHighestResults() {
    if (allResults.rbegin() == allResults.rend()) {
        return 0;
    }
    return allResults.rbegin()->first;
}

std::optional<resultContainer> ResultStorage::getResults(uint64_t lowerBound) {
    if (!hasResults(lowerBound)) {
        return std::nullopt;
    }
    return allResults[lowerBound];
}