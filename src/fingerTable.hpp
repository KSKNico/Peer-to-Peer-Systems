#pragma once

#include <array>
#include <optional>

#include "hash.hpp"

class FingerTable {
   public:
    FingerTable() = delete;
    FingerTable(const Poco::Net::SocketAddress& ownAddress);

    Poco::Net::SocketAddress getSuccessor() const;
    Poco::Net::SocketAddress getPredecessor() const;
    Poco::Net::SocketAddress getFinger(std::size_t index) const;

    // returns the entry in the finger table that is closest to the finger and still in front of it
    // so that the hash is not "overshot"
    Poco::Net::SocketAddress getClosestPrecedingFinger(const Hash& hash) const;

    // returns the hash value the finger at index should point to
    // index must be in the range [0, Hash::HASH_BIT_SIZE)
    Hash getFingerHashValue(std::size_t index) const;

    Hash getOwnHash() const;

    void updateWithAddress(const Poco::Net::SocketAddress& address);

    // removes the address from the finger table
    // is the reverse of updateWithAddress
    void removeAddress(const Poco::Net::SocketAddress& address);

    void updateSuccessor(const Poco::Net::SocketAddress& successor);

    // only sets the predecessor if the new predecessor is closer to the peer's own address
    void updatePredecessor(const Poco::Net::SocketAddress& predecessor);

    static const std::size_t FINGER_TABLE_SIZE = Hash::HASH_BIT_SIZE;

   private:
    void setSuccessor(const Poco::Net::SocketAddress& successor);
    void setPredecessor(const Poco::Net::SocketAddress& predecessor);

    const Poco::Net::SocketAddress ownAddress;
    const Hash ownHash;
    Poco::Net::SocketAddress successor;
    Poco::Net::SocketAddress predecessor;
    std::array<Poco::Net::SocketAddress, Hash::HASH_BIT_SIZE> table;
};