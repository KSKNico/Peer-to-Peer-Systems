#include "hash.hpp"

Poco::SHA2Engine256 Hash::engine;

Hash::Hash(const std::array<uint8_t, HASH_BYTE_SIZE> &bytes) : bytes(bytes) {
}

Hash Hash::hashSocketAddress(Poco::Net::SocketAddress const &address) {
    engine.update(address.toString());
    auto d = engine.digest();
    std::array<uint8_t, HASH_BYTE_SIZE> bytes;
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        bytes[i] = d[i];
    }
    return Hash(bytes);
}

bool Hash::compareHashes(Hash const &hash1, Hash const &hash2) {
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        if (hash1.bytes[i] != hash2.bytes[i]) {
            return false;
        }
    }
    return true;
}


std::string Hash::toString() const {
    std::stringstream ss("");
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) bytes[i];
    }
    return ss.str();
}

std::size_t Hash::Hasher::operator()(const Hash &hash) const {
    // Use the first sizeof(std::size_t) bytes of the hash value as the hash code.
    // This assumes that sizeof(std::size_t) <= HASH_BYTE_SIZE.
    std::size_t hashCode = 0;
    std::memcpy(&hashCode, hash.bytes.data(), sizeof(hashCode));
    return hashCode;
}