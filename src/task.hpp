#pragma once

#include "hash.hpp"
#include "networking/message.hpp"

class Task {
   public:
    virtual void processMessage(const Message& message);
};

class IDTask : public Task {
   public:
    void processMessage(const Message& message) override;
};

class FindTask : public Task {
   public:
    FindTask(const Hash& target);
    void processMessage(const Message& message) override;

   private:
    Hash target;
    Poco::Net::SocketAddress lastSentTo;
};