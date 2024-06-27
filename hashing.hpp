#pragma once

#include "Poco/SHA2Engine.h"
#include "Poco/Net/SocketAddress.h"
#include <cstdint>
#include <sstream>
#include <iomanip>
constexpr int HASH_BIT_SIZE = 256;

// this should be set so that 2^HASH_EXPONENT = HASH_BIT_SIZE
constexpr int HASH_EXPONENT = 8;

constexpr int HASH_BYTE_SIZE = HASH_BIT_SIZE / 8;

using Hash = std::array<uint8_t, HASH_BYTE_SIZE>;

Hash hashSocketAddress(Poco::Net::SocketAddress const &addr);

bool compareHashes(Hash const &hash1, Hash const &hash2);

std::string hashToString(Hash const &hash);