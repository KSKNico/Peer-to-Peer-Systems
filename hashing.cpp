#include "hashing.hpp"

static Poco::SHA2Engine256 engine;

Hash hashSocketAddress(Poco::Net::SocketAddress const &address) {
    engine.update(address.toString());
    auto d = engine.digest();
    Hash returnArray;
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        returnArray[i] = d[i];
    }
    return returnArray;
}

bool compareHashes(Hash const &hash1, Hash const &hash2) {
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        if (hash1[i] != hash2[i]) {
            return false;
        }
    }
    return true;
}


std::string hashToString(Hash const &hash) {
    std::stringstream ss("");
    for (int i = 0; i < HASH_BYTE_SIZE; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}