#include <iostream>
#include <fstream>
#include "peerConfig.hpp"
#include "peer.hpp"
#include "Poco/Net/SocketAddress.h"


int main() {
    auto addr = Poco::Net::SocketAddress("127.0.0.1:5002");
    std::vector<Poco::Net::SocketAddress> addr_v = {addr} ;

    auto ownAddress = Poco::Net::SocketAddress("127.0.0.1:5001");
    Peer(ownAddress, addr_v);
}