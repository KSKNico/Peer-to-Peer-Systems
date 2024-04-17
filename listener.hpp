#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include "connection.hpp"

class Listener
{
    public:
    int serverSocket;
    struct sockaddr_in address;
    Listener();
    ~Listener();

    /* Accepts an incoming socket connection and returns it */
    Connection acceptConnection();
};