#pragma once

#include "networking/message.hpp"

class Task {
    virtual void processMessage(const Message& message);
    virtual ~Task() = default;
};

class FindTask : public Task {
};