#pragma once

#include "json.hpp"
#include <cstdint>
#include <string>
#include <fstream>
#include <arpa/inet.h>

class PeerConfig {
    private:
    uint16_t listeningPort;
    std::vector<sockaddr_in> knownHosts;
    void loadConfig(std::string filename);

    public:
    PeerConfig(std::string filename);

    PeerConfig(const PeerConfig& ) = delete;
    PeerConfig(PeerConfig&&) = delete;

    uint16_t getListeningPort() const;
    std::vector<sockaddr_in> getKnownHosts() const;

    /* Converts an address string to a sockaddr struct */
    static sockaddr_in convertToAddress(std::string addressString);

};