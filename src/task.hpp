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
    virtual TaskState getState() const;

   protected:
    TaskState state = TaskState::UNINITIALIZED;
    FingerTable& fingerTable;
    ConnectionManager& connectionManager;
};

class FindTask : public Task {
   public:
    FindTask(const Hash& target, FingerTable& fingerTable, ConnectionManager& connectionManager, std::optional<Poco::Net::SocketAddress> nextHop);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;
    std::optional<Poco::Net::SocketAddress> getTargetAddress() const;

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
    JoinTask(const Poco::Net::SocketAddress& ownAddress, const Poco::Net::SocketAddress& joinAddress,
             FingerTable& fingerTable, ConnectionManager& connectionManager);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    Poco::Net::SocketAddress ownAddress;
    Poco::Net::SocketAddress joinAddress;
    FindTask findTask;
};

// p asks the successor for its predecessor
// if the predecessor of the successor (n) is not equal to p then
// succ(p) = n
// and inform n that p is its predecessor
// he might have a closer predecessor then p is discarded
class StabilizeTask : public Task {
   public:
    StabilizeTask(const Poco::Net::SocketAddress& ownAddress,
                  FingerTable& fingerTable, ConnectionManager& connectionManager);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    bool messageSent = false;
    Poco::Net::SocketAddress currentSuccessor;
    Poco::Net::SocketAddress ownAddress;
    std::optional<Poco::Net::SocketAddress> predecessorOfSuccessor;
};

class FixFingersTask : public Task {
   public:
    FixFingersTask(const Poco::Net::SocketAddress& ownAddress,
                   FingerTable& fingerTable, ConnectionManager& connectionManager);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    std::vector<FindTask> findTasks;
    Poco::Net::SocketAddress ownAddress;
};

class CheckPredecessorTask : public Task {
   public:
    CheckPredecessorTask(const Poco::Net::SocketAddress& ownAddress,
                         FingerTable& fingerTable, ConnectionManager& connectionManager);
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    Poco::Net::SocketAddress ownAddress;
    std::optional<Poco::Net::SocketAddress> predecessor;
};