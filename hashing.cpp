#include "hashing.hpp"

static Poco::SHA2Engine256 engine;

std::array<uint8_t, HASH_BYTE_SIZE> hashSocketAddress(Poco::Net::SocketAddress const &address) {
    engine.update(address.toString());
    auto d = engine.digest();
    std::array<uint8_t, HASH_BYTE_SIZE> returnArray;
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        returnArray[i] = d[i];
    }
    return returnArray;
}

bool compareHashes(std::array<uint8_t, HASH_BYTE_SIZE> const &hash1, std::array<uint8_t, HASH_BYTE_SIZE> const &hash2) {
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        if (hash1[i] != hash2[i]) {
            return false;
        }
    }
    return true;
}