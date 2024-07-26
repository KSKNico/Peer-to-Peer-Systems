#pragma once

#include <array>

#include "hash.hpp"

class FingerTable {
   public:
    FingerTable(const Poco::Net::SocketAddress& ownAddress);

    Poco::Net::SocketAddress getSuccessor() const;
    Poco::Net::SocketAddress getPredecessor() const;
    Poco::Net::SocketAddress getFinger(std::size_t index) const;

    // returns the entry in the finger table that is closest to the finger and still in front of it
    // so that the hash is not "overshot"
    Poco::Net::SocketAddress getClosestPrecedingFinger(const Hash& hash) const;

    // returns the size of the finger table
    std::size_t getSize() const;

    // returns the hash value the finger at index should point to
    // index must be in the range [0, Hash::HASH_BIT_SIZE)
    Hash getFingerHashValue(std::size_t index) const;

    Hash getOwnHash() const;

    void updateWithAddress(const Poco::Net::SocketAddress& address);

   private:
    const std::size_t FINGER_TABLE_SIZE = Hash::HASH_BIT_SIZE;
    const Poco::Net::SocketAddress ownAddress;
    const Hash ownHash;
    Poco::Net::SocketAddress successor;
    Poco::Net::SocketAddress predecessor;
    std::array<Poco::Net::SocketAddress, Hash::HASH_BIT_SIZE> table;
};