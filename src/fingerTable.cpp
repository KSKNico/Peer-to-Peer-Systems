#include "fingerTable.hpp"

FingerTable::FingerTable(const Poco::Net::SocketAddress &ownAddress) : ownAddress(ownAddress),
                                                                       ownHash(Hash(ownAddress)),
                                                                       successor(ownAddress),
                                                                       predecessor(ownAddress) {
    for (std::size_t i = 0; i < FINGER_TABLE_SIZE; i++) {
        table[i] = ownAddress;
    }
    table[0] = successor;
}

Poco::Net::SocketAddress FingerTable::getClosestPrecedingFinger(const Hash &hash) const {
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

std::size_t FingerTable::getSize() const {
    return FINGER_TABLE_SIZE;
}
