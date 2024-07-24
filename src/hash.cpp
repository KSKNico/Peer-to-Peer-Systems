#include "hash.hpp"

#include <algorithm>
#include <cassert>

Poco::SHA2Engine256 Hash::engine;

Hash::Hash(HashType hashValue) : hashValue(hashValue) {}

Hash::Hash(const Poco::Net::SocketAddress &address) : hashValue(hashSocketAddress(address).getHashValue()) {}

Hash Hash::hashSocketAddress(Poco::Net::SocketAddress const &address) {
    engine.update(address.toString());
    auto d = engine.digest();
    HashType hashValue = 0;
    for (long unsigned int i = 0; i < sizeof(HashType); i++) {
        hashValue = (hashValue << 8) | d[i];
    }
    return Hash(hashValue);
}

Hash Hash::hashInterval(unsigned long long intervalStart) {
    engine.update(std::to_string(intervalStart));
    auto d = engine.digest();
    HashType hashValue = 0;
    for (long unsigned int i = 0; i < sizeof(HashType); i++) {
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
    HashType hashValue;
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

Hash Hash::fromExponent(const uint8_t exponent) {
    assert(exponent < HASH_BIT_SIZE);
    HashType hashValue = 1;
    hashValue <<= exponent;
    return Hash(hashValue);
}

Hash Hash::operator-(const Hash &other) const {
    return Hash(this->hashValue - other.hashValue);
}

Hash Hash::distance(Hash const &other) const {
    return other - *this;
}

/*
bool Hash::isBefore(Hash const &other) const {
    return this->distance(other) == (other - *this).hashValue;
}
*/

Hash::HashType Hash::getHashValue() const {
    return this->hashValue;
}

/*
std::size_t std::hash<Poco::Net::SocketAddress>::operator()(const Poco::Net::SocketAddress &addr) const {
    return Hash::hashSocketAddress(addr).getHashValue();
}
*/

std::size_t Hash::SocketAddressHasher::operator()(const Poco::Net::SocketAddress &addr) const {
    return Hash::hashSocketAddress(addr).getHashValue();
}