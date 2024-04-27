#include <iostream>
#include <fstream>
#include "peer.hpp"
#include "json.hpp"


// loads config from config.json
nlohmann::json loadConfig() {
    std::ifstream file("config.json");
    nlohmann::json data = nlohmann::json::parse(file); 
    file.close();
    return data;
}

sockaddr_in convertToAddress(std::string addressString)
{
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

int main() {
    auto config = loadConfig();
    uint16_t port = config["listeningPort"].get<uint16_t>();
    auto knownHosts = config["known_hosts"].get<std::vector<std::string>>();
    auto hosts = std::vector<sockaddr_in>();

    for (auto& host : knownHosts)
    {
        hosts.push_back(convertToAddress(host));
    }
    return 0;
}