#include "peerConfig.hpp"


PeerConfig::PeerConfig(std::string filename) {
    loadConfig("peerConfig.json");
}

void PeerConfig::loadConfig(std::string filename) {
    std::ifstream file(filename);
    nlohmann::json data = nlohmann::json::parse(file);
    file.close();
    listeningPort = data["listeningPort"].get<uint16_t>();
    auto knownHostsStrings = data["knownHosts"].get<std::vector<std::string>>();
    for (auto& host : knownHostsStrings)
    {
        knownHosts.push_back(convertToAddress(host));
    }
}

uint16_t PeerConfig::getListeningPort() const {
    return listeningPort;
}

std::vector<sockaddr_in> PeerConfig::getKnownHosts() const {
    return knownHosts;
}

sockaddr_in PeerConfig::convertToAddress(std::string addressString) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    // the addressString has the format x.x.x.x:yyyy
    auto colon = addressString.find(':');
    auto ip = addressString.substr(0, colon);
    auto port = addressString.substr(colon + 1);
    address.sin_port = htons(std::stoi(port));
    inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
    return address;
}