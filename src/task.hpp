#pragma once

#include "Poco/Net/SocketAddress.h"
#include "fingerTable.hpp"
#include "hash.hpp"
#include "networking/connectionManager.hpp"
#include "networking/message.hpp"

enum class TaskState { UNINITIALIZED,
                       RUNNING,
                       FINISHED };

class Task {
   public:
    Task(FingerTable& fingerTable, ConnectionManager& connectionManager);
    virtual ~Task() = default;
    virtual void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) = 0;
    virtual void update() = 0;
    virtual void init() = 0;

   protected:
    TaskState state;
    FingerTable& fingerTable;
    ConnectionManager& connectionManager;
};

class FindTask : public Task {
   public:
    FindTask(const Hash& target, FingerTable& fingerTable, ConnectionManager& connectionManager);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    const Hash target;
    Poco::Net::SocketAddress lastSentTo;

    // this is the next hop to send a find message to
    std::optional<Poco::Net::SocketAddress> nextHop;

    // target address that is responsible for target
    std::optional<Poco::Net::SocketAddress> targetAddress;
};

// is supposed to run on the peer that is joining a network
class JoinTask : public Task {
   public:
    // requires the address of the peer where the join takes place
    // first a find is conducted on the own address to find the successor of this peer
    JoinTask(const Poco::Net::SocketAddress& ownAddress, const Poco::Net::SocketAddress& joinAddress, FingerTable& fingerTable, ConnectionManager& connectionManager);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    Poco::Net::SocketAddress joinAddress;
    FindTask findTask;
};