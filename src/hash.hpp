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
    static Poco::SHA2Engine256 engine;
    using HashType = uint64_t;
    static constexpr int HASH_BIT_SIZE = sizeof(HashType) * 8;
    static constexpr int HASH_BYTE_SIZE = sizeof(HashType);

    std::string toString() const;

    static Hash fromString(std::string& str);

    static Hash hashSocketAddress(Poco::Net::SocketAddress const& addr);

    static Hash hashInterval(unsigned long long intervalStart);

    // calculates distance between two hashes
    // this wraps around the circle
    Hash distance(Hash const& other) const;

    // creates a hash by calculating 2**exponent
    static Hash fromExponent(const uint8_t exponent);

    static bool compareHashes(Hash const& hash1, Hash const& hash2);

    bool isBefore(Hash const& other) const;

    struct Hasher {
        // returns a hash type compatible with map types
        std::size_t operator()(const Hash& hash) const;
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

   private:
    uint64_t hashValue;
    Hash(uint64_t hashValue);
};