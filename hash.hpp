#pragma once

#include "Poco/SHA2Engine.h"
#include "Poco/Net/SocketAddress.h"
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <array>
#include <memory>
#include <cstring>

class Hash {
public:

    static Poco::SHA2Engine256 engine;
    static constexpr int HASH_BIT_SIZE = 256;

    // this should be set so that 2^HASH_EXPONENT = HASH_BIT_SIZE
    static constexpr int HASH_EXPONENT = 8;
    static constexpr int HASH_BYTE_SIZE = HASH_BIT_SIZE / 8;

    std::array<uint8_t, HASH_BYTE_SIZE> bytes;

    Hash(const std::array<uint8_t, HASH_BYTE_SIZE>& bytes);

    std::string toString() const;

    static Hash hashSocketAddress(Poco::Net::SocketAddress const &addr);

    static bool compareHashes(Hash const &hash1, Hash const &hash2);

    struct Hasher {
        std::size_t operator()(const Hash& hash) const;
    };

    bool operator==(const Hash& other) const;
};