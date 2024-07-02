#pragma once

#include <array>
#include "hash.hpp"
#include "Poco/Net/SocketStream.h"

class FingerTable {
    public:
        /* void addFinger();
        void removeFinger();
        void updateFinger();
        void getFinger();*/
        constexpr static int FINGER_TABLE_SIZE = Hash::HASH_BIT_SIZE;
    private:
        std::array<Hash, FINGER_TABLE_SIZE> fingers;
        Hash predecessor;
        Hash successor;
};