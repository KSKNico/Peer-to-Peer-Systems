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
    Listener(uint16_t port);
    ~Listener();

    /* Accepts an incoming socket connection and returns it. If there is no connection it blocks. */
    Connection acceptConnection() const;

    /* Checks whether there is an incoming connection */
    bool pollForConnection() const;

    int getSocket() const;
};