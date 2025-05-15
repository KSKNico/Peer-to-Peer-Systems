#pragma once

#include "Poco/Net/SocketAddress.h"
#include "fingerTable.hpp"
#include "hash.hpp"
#include "networking/connectionManager.hpp"
#include "networking/message.hpp"
#include "resultStorage.hpp"

enum class TaskState { UNINITIALIZED,
                       RUNNING,
                       FINISHED };

class Task {
   public:
    Task(FingerTable& fingerTable, 
        ConnectionManager& connectionManager, 
        const Poco::Net::SocketAddress& ownAddress);
    virtual ~Task() = default;
    Task() = delete;
    virtual bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) = 0;
    virtual void update() = 0;
    virtual void init() = 0;
    virtual TaskState getState() const;


   protected:
    TaskState state = TaskState::UNINITIALIZED;
    FingerTable& fingerTable;
    ConnectionManager& connectionManager;
    Poco::Net::SocketAddress ownAddress;
    unsigned int messagesSent = 0;
};

// task that finds the peer responsible for the target hash
class FindTask : public Task {
   public:
    FindTask(const Hash& target,
            FingerTable& fingerTable, 
            ConnectionManager& connectionManager, 
            const Poco::Net::SocketAddress& ownAddress,
            std::optional<Poco::Net::SocketAddress> initialPeer = std::nullopt);
    bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;
    std::optional<Poco::Net::SocketAddress> getTargetAddress() const;

   private:
    const Hash target;
    Poco::Net::SocketAddress lastSentTo;

    // this is the initial peer for joining the network if the joining peer has 
    // no fingertable yet
    std::optional<Poco::Net::SocketAddress> initialPeer;

    // target address that is responsible for target
    std::optional<Poco::Net::SocketAddress> targetAddress;

    // this is the peer that the find task needs to send a find message to
    Poco::Net::SocketAddress sendTo;
};

// is supposed to run on the peer that is joining a network
class JoinTask : public Task {
   public:
    // requires the address of the peer where the join takes place
    // first a find is conducted on the own address to find the successor of this peer
    JoinTask(const Poco::Net::SocketAddress& joinAddress,
             FingerTable& fingerTable, 
             ConnectionManager& connectionManager, 
             const Poco::Net::SocketAddress& ownAddress);
    bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
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
    StabilizeTask(FingerTable& fingerTable, 
                  ConnectionManager& connectionManager,
                  const Poco::Net::SocketAddress& ownAddress);
    bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    bool messageSent = false;
    Poco::Net::SocketAddress currentSuccessor;
    std::optional<Poco::Net::SocketAddress> predecessorOfSuccessor;
};

class FixFingersTask : public Task {
   public:
    FixFingersTask(FingerTable& fingerTable, 
                   ConnectionManager& connectionManager, 
                   const Poco::Net::SocketAddress& ownAddress);
    bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    std::vector<FindTask> findTasks;
};

class CheckPredecessorTask : public Task {
   public:
    CheckPredecessorTask(FingerTable& fingerTable, 
                         ConnectionManager& connectionManager, 
                         const Poco::Net::SocketAddress& ownAddress);
    bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
    void update() override;
    void init() override;

   private:
    std::optional<Poco::Net::SocketAddress> predecessor;
};

class CalculatePrimeTask : public Task {
    public:
     CalculatePrimeTask(FingerTable& fingerTable, 
                        ConnectionManager& connectionManager, 
                        const Poco::Net::SocketAddress& ownAddress,
                        std::optional<uint64_t> lowerBound);
     bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
     void update() override;
     void init() override;
    
    private:
     std::optional<uint64_t> toCalculate;
     bool calculated = false;
     std::vector<uint64_t> primes;
};


// this task searched through the entire chord network to find the highest unknown interval
// this interval can then be calculated by the 
// 1. look for the highest unknown interval in your own result storage
// 2. do a find in the network to find the responsible peer
// 3. ask the peer for its highest unknown interval
// 4. if the peer has a higher unknown interval then the one you have, then ask the peer for its highest unknown interval
class GetHighestUnknownIntervalTask : public Task {
    public:
     GetHighestUnknownIntervalTask(ResultStorage& resultStorage,
                                   FingerTable& fingerTable, 
                                   ConnectionManager& connectionManager, 
                                   const Poco::Net::SocketAddress& ownAddress);
     bool processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) override;
     void update() override;
     void init() override;

     void updateNextHighestInterval(resultType interval);
    
    private:
     ResultStorage& resultStorage;
     std::optional<resultType> highestInterval;
     resultType currentlyHighestInterval = 0;
     resultContainer currentlyHighestResults;
     
     std::optional<FindTask> findTask;
};