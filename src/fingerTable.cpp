#include "fingerTable.hpp"

#include <cassert>

FingerTable::FingerTable(const Poco::Net::SocketAddress &ownAddress) : ownAddress(ownAddress),
                                                                       ownHash(Hash(ownAddress)),
                                                                       successor(ownAddress),
                                                                       predecessor(ownAddress) {
    table.fill(ownAddress);
}

Poco::Net::SocketAddress FingerTable::getClosestPrecedingFinger(const Hash &hash) const {
    if (hash == ownHash) {
        return ownAddress;
    }

    if (hash.isBetween(ownHash, Hash(successor))) {
        return successor;
    }

    return *std::min_element(table.begin(), table.end(), [hash](const Poco::Net::SocketAddress &lhs, const Poco::Net::SocketAddress &rhs) {
        return Hash(lhs).distance(hash) < Hash(rhs).distance(hash);
    });
}

Hash FingerTable::getFingerHashValue(std::size_t index) const {
    return ownHash + Hash::fromExponent(index);
}

Poco::Net::SocketAddress FingerTable::getFinger(std::size_t index) const {
    return table[index];
}

Hash FingerTable::getOwnHash() const {
    return ownHash;
}

Poco::Net::SocketAddress FingerTable::getSuccessor() const {
    return successor;
}

Poco::Net::SocketAddress FingerTable::getPredecessor() const {
    return predecessor;
}

void FingerTable::updateWithAddress(const Poco::Net::SocketAddress &address) {
    assert(address != ownAddress);
    Hash addressHash(address);
    for (unsigned int i = 0; i < FINGER_TABLE_SIZE; i++) {
        auto fingerHash = Hash::fromExponent(i) + ownHash;
        auto fingerPointedHash = Hash(table[i]);
        if (addressHash.isBetween(fingerHash, fingerPointedHash)) {
            table[i] = address;
        }
    }
}
