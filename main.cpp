#include <iostream>
#include <fstream>
#include "peer.hpp"
#include "json.hpp"
#include "peerConfig.hpp"


int main() {
    auto config = PeerConfig("peerConfig.json");
    auto peer = Peer(config.getListeningPort(), config.getKnownHosts());
    return 0;
}