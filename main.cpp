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

int main() {
    auto config = loadConfig();
    int port = config["port"].get<int>();
    auto knownHosts = config["known_hosts"].get<std::vector<std::string>>();
    Peer(knownHosts, port);
    return 0;
}