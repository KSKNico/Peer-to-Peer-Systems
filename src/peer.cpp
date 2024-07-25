#include "peer.hpp"

#include <iostream>

#include "Poco/Net/SocketReactor.h"
#include "resultHandler.hpp"

Peer::Peer(Poco::Net::SocketAddress ownAddress, Poco::Net::SocketAddress remoteAddress) : Peer(ownAddress) {}

Peer::Peer(Poco::Net::SocketAddress ownAddress) : connectionManager(ownAddress), fingerTable(ownAddress), taskManager(connectionManager, fingerTable) {}