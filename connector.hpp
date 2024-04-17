#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include "connection.hpp"


class Connector {
    private:
        /* data */
    public:
        Connector();
        
        Connection connectTo(sockaddr_in address);
};