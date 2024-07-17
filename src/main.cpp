#include <iostream>
#include <future>
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetworkInterface.h"

constexpr int PORT = 5000;

// pass command line arguments to the program with the current IP and potentially the remote IP plus port
// first argument own IP
// second argument remote IP (optional)

int main(int argc, char* argv[]) {
    std::string ownIPAddress_str = "";
    std::string remoteIPAddress_str = "";
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <own IP> [<remote IP>]" << std::endl;
        return 1;
    } else if (argc == 2) {
        ownIPAddress_str = argv[1];
    } else if (argc == 3) {
        ownIPAddress_str = argv[1];
        remoteIPAddress_str = argv[2];
    } else {
        std::cerr << "Usage: " << argv[0] << " <own IP> [<remote IP>]" << std::endl;
        return 1;
    }

    Poco::Net::SocketAddress ownSocketAddress(Poco::Net::IPAddress(ownIPAddress_str), PORT);
    Poco::Net::SocketAddress remoteSocketAddress(Poco::Net::IPAddress(remoteIPAddress_str), PORT);
}