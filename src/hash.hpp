#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>

#include "Poco/Net/SocketAddress.h"
#include "Poco/SHA2Engine.h"

class Hash {
   public:
    static Poco::SHA2Engine engine;
    using HashType = uint64_t;
    static constexpr int HASH_BIT_SIZE = sizeof(HashType) * 8;
    static constexpr int HASH_BYTE_SIZE = sizeof(HashType);

    Hash(Poco::Net::SocketAddress const& addr);

    std::string toString() const;

    static Hash fromString(const std::string& str);

    static Hash hashSocketAddress(Poco::Net::SocketAddress const& addr);

    static Hash hashInterval(unsigned long long intervalStart);

    // returns true if this hash is in the interval (start, end]
    // this means "end" is responsible for this hash
    bool isBetween(Hash const& start, Hash const& end) const;

    // calculates the distance from this hash to the other
    // the distance is not commutative as it is a ring that can only be traversed in one direction
    // so distance(a, b) != distance(b, a)
    Hash distance(Hash const& other) const;

    // creates a hash by calculating 2**exponent
    static Hash fromExponent(const uint8_t exponent);

    static bool compareHashes(Hash const& hash1, Hash const& hash2);

    // bool isBefore(Hash const& other) const;

    struct SocketAddressHasher {
        std::size_t operator()(const Poco::Net::SocketAddress& addr) const;
    };

    bool operator==(const Hash& other) const;
    bool operator!=(const Hash& other) const;
    bool operator<(const Hash& other) const;
    bool operator>(const Hash& other) const;
    bool operator<=(const Hash& other) const;
    bool operator>=(const Hash& other) const;

    // this wraps around if the sum is greater than 2^HASH_BIT_SIZE
    Hash operator+(const Hash& other) const;

    // this wraps around if the difference is negative
    Hash operator-(const Hash& other) const;

    HashType getHashValue() const;

    static Hash _hashFromHashValue(HashType hashValue);

   private:
    uint64_t hashValue;
    Hash(uint64_t hashValue);
    Hash() = delete;
};