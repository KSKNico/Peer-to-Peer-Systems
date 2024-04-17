#pragma once
#include "arpa/inet.h"
#include "unistd.h"
#include <string>

class Connection
{
    private:
    int socket;
    struct sockaddr_in address;
    public:
    Connection(int socket, struct sockaddr_in address);
    ~Connection();

    void sendData(std::string data);
    std::string receiveData();
};