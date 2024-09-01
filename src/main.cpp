#include <future>
#include <iostream>
#include "peer.hpp"

#include "Poco/Net/NetworkInterface.h"
#include "Poco/Net/SocketAddress.h"

constexpr int PORT = 5000;

// pass command line arguments to the program with the current IP and
// potentially the remote IP plus port first argument own IP second argument
// remote IP (optional)

int main(int argc, char* argv[]) {
    std::string ownIPAddress_str = "";
    std::string remoteIPAddress_str = "";
    std::unique_ptr<Peer> peer;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <own IP> [<remote IP>]" << std::endl;
        return 1;
    } else if (argc == 2) {
        ownIPAddress_str = argv[1];
        peer = std::make_unique<Peer>(Poco::Net::SocketAddress(ownIPAddress_str));
    } else if (argc == 3) {
        ownIPAddress_str = argv[1];
        remoteIPAddress_str = argv[2];
        peer = std::make_unique<Peer>(Poco::Net::SocketAddress(ownIPAddress_str), Poco::Net::SocketAddress(remoteIPAddress_str));
    } else {
        std::cerr << "Usage: " << argv[0] << " <own IP> [<remote IP>]" << std::endl;
        return 1;
    }

    while (true) {
        peer->update();
        peer->printConnections();
    }

    
}