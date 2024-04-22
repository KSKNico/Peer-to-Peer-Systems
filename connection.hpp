#pragma once
#include "arpa/inet.h"
#include "unistd.h"
#include <vector>
#include <cstddef>

class Connection
{
    private:
    int socket;
    struct sockaddr_in address;
    public:
    Connection(int socket, struct sockaddr_in address);
    ~Connection();

    void sendData(std::vector<std::byte> data) const;

    std::vector<std::byte> receiveData(uint32_t size = 1024) const;
};