#include "fingerTable.hpp"

#include <cassert>
#include "spdlog/spdlog.h"

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

void FingerTable::setSuccessor(const Poco::Net::SocketAddress &successor) {
    spdlog::get(this->ownAddress.toString())->debug("Setting successor to {}", successor.toString());

    if (this->successor == ownAddress) {
        // this means that the peer is the only peer in the network
        this->successor = successor;
        
        // if the predecessor is the peer itself then the predecessor must be the new successor
        if (this->predecessor == ownAddress) {
            this->predecessor = successor;
        }
        return;
    }

    if (Hash(successor).isBetween(ownHash, Hash(this->successor))) {
        this->successor = successor;
    }
}

void FingerTable::setPredecessor(const Poco::Net::SocketAddress &predecessor) {
    // if there is no successor and no predecessor, set the predecessor to the new predecessor
    if (this->successor == ownAddress && this->predecessor == ownAddress) {
        this->predecessor = predecessor;
        this->successor = predecessor;
        return;
    }

    // if the current predecessor is not set, set it to the new predecessor
    if (this->predecessor == ownAddress) {
        assert(Hash(ownAddress).isBetween(Hash(predecessor), Hash(this->successor)));
        this->predecessor = predecessor;
        return;
    }

    if (Hash(predecessor).isBetween(Hash(this->predecessor), ownHash)) {
        this->predecessor = predecessor;
        return;
    }
}

Poco::Net::SocketAddress FingerTable::getPredecessor() const {
    return predecessor;
}

void FingerTable::updateWithAddress(const Poco::Net::SocketAddress &address) {
    // assert(address != ownAddress);
    Hash addressHash(address);
    for (unsigned int i = 0; i < FINGER_TABLE_SIZE; i++) {
        auto fingerHash = Hash::fromExponent(i) + ownHash;
        auto fingerPointedHash = Hash(table[i]);
        if (addressHash.isBetween(fingerHash, fingerPointedHash)) {
            table[i] = address;
        }
    }
}

void FingerTable::removeAddress(const Poco::Net::SocketAddress &address) {
    Poco::Net::SocketAddress lastAddress = ownAddress;
    for (unsigned int i = FINGER_TABLE_SIZE - 1; i <= FINGER_TABLE_SIZE; --i) {
        if (table[i] == address) {
            table[i] = lastAddress;
        } else {
            lastAddress = table[i];
        }
    }
}
