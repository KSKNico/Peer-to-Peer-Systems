#pragma once

#include "hash.hpp"
#include "myConnectionHandler.hpp"
#include <unordered_map>
#include <memory>
#include <mutex>

class Connections {
public:
    void addConnection(Hash const &hash, MyConnectionHandler* connection);
    MyConnectionHandler* getConnection(Hash const &hash);
    void removeConnection(Hash const &hash);
    void clearConnections();
    std::vector<Hash> getHashes();
    std::vector<MyConnectionHandler*> getConnections();
    
private:
    std::mutex mutex;
    std::unordered_map<Hash, MyConnectionHandler*, Hash::Hasher> connections;
};