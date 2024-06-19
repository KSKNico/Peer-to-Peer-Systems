#pragma once

#include <array>
#include "hashing.hpp"
#include "Poco/Net/SocketStream.h"

class FingerTable {
    public:
        /* void addFinger();
        void removeFinger();
        void updateFinger();
        void getFinger();*/
    private:
        std::array<Poco::Net::SocketStream, HASH_EXPONENT> fingerTable;
};