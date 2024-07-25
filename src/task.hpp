#pragma once

#include "Poco/Net/SocketAddress.h"
#include "hash.hpp"
#include "networking/message.hpp"

class Task {
   public:
    virtual ~Task() = default;
    virtual void processMessage(const Message& message) = 0;

   protected:
    bool isFinished = false;
};

class FindTask : public Task {
   public:
    FindTask(const Hash& target);
    void processMessage(const Message& message) override;

   private:
    Hash target;
    Poco::Net::SocketAddress lastSentTo;
};

// is supposed to run on the peer that is joining a network
class JoinTask : public Task {
   public:
    // requires the address of the peer where the join takes place
    JoinTask(const Poco::Net::SocketAddress& address);
    void processMessage(const Message& message) override;

   private:
};

class JoinResponseTask : public Task {
   public:
    JoinResponseTask(const Poco::Net::SocketAddress& joinerAddress);
    void processMessage(const Message& message) override;

   private:
    FindTask findTask;
};