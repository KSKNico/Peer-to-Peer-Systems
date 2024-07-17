#include "hash.hpp"

#include <algorithm>

Poco::SHA2Engine256 Hash::engine;

Hash::Hash(uint64_t hashValue) : hashValue(hashValue) {}

Hash Hash::hashSocketAddress(Poco::Net::SocketAddress const &address) {
    engine.update(address.toString());
    auto d = engine.digest();
    uint64_t hashValue = 0;
    for (int i = 0; i < sizeof(uint64_t); i++) {
        hashValue = (hashValue << 8) | d[i];
    }
    return Hash(hashValue);
}

Hash Hash::hashInterval(unsigned long long intervalStart) {
    engine.update(std::to_string(intervalStart));
    auto d = engine.digest();
    uint64_t hashValue = 0;
    for (int i = 0; i < sizeof(uint64_t); i++) {
        hashValue = (hashValue << 8) | d[i];
    }
    return Hash(hashValue);
}

bool Hash::compareHashes(Hash const &hash1, Hash const &hash2) {
    return hash1.hashValue == hash2.hashValue;
}


std::string Hash::toString() const {
    std::stringstream ss;
    ss << std::hex << this->hashValue;
    return ss.str();
}

Hash Hash::fromString(std::string &str) {
    std::stringstream ss;
    ss << std::hex << str;
    uint64_t hashValue;
    ss >> hashValue;
    return Hash(hashValue);
}


bool Hash::operator==(const Hash &other) const {
    return compareHashes(*this, other);
}

Hash Hash::operator+(const Hash &other) const {
    return Hash(this->hashValue + other.hashValue);
}

bool Hash::operator<(const Hash &other) const {
    return this->hashValue < other.hashValue;
}

bool Hash::operator!=(const Hash &other) const {
    return !(*this == other);
}

bool Hash::operator>(const Hash &other) const {
    return other < *this;
}

bool Hash::operator<=(const Hash &other) const {
    return !(*this > other);
}

bool Hash::operator>=(const Hash &other) const {
    return !(*this < other);
}

std::size_t Hash::Hasher::operator()(const Hash& hash) const {
    return hash.hashValue;
}

Hash Hash::fromExponent(const uint8_t exponent) {
    uint64_t hashValue = 1;
    hashValue <<= exponent;
    return Hash(hashValue);
}

Hash Hash::operator-(const Hash &other) const {
    return Hash(this->hashValue - other.hashValue);
}

Hash Hash::distance(Hash const &other) const {
    // return the smaller distance!
    return std::min((*this-other).hashValue, (other-*this).hashValue);
}

bool Hash::isBefore(Hash const &other) const {
    return this->distance(other) == (other - *this).hashValue;
}