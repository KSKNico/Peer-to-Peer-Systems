#pragma once

#include "hash.hpp"
#include "networking/message.hpp"

class Task {
   public:
    virtual ~Task() = default;
    virtual void processMessage(const Message& message) = 0;
};

class FindTask : public Task {
   public:
    FindTask(const Hash& target);
    void processMessage(const Message& message) override;

   private:
    Hash target;
    Poco::Net::SocketAddress lastSentTo;
};