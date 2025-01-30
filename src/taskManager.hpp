#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "task.hpp"
#include "timing.hpp"
#include "spdlog/spdlog.h"

// contains the task and timing information about it
using TaskPair = std::pair<std::unique_ptr<Task>, Timing>;

class TaskManager {
   public:
    TaskManager(const Poco::Net::SocketAddress& ownAddress, 
    ConnectionManager& connectionManager, 
    FingerTable& fingerTable);
    void addTask(std::unique_ptr<Task>&& task);
    void update();
    void removeCompletedTasks();
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message);

    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;
    TaskManager(const TaskManager&&) = delete;
    TaskManager& operator=(const TaskManager&&) = delete;

   private:
    void launchPeriodicTasks();

    const std::chrono::seconds stabilizeInterval = std::chrono::seconds(1);
    const std::chrono::seconds fixFingersInterval = std::chrono::seconds(5);
    const std::chrono::seconds checkPredecessorInterval = std::chrono::seconds(10);
    const std::chrono::seconds taskTimeout = std::chrono::seconds(5);

    std::chrono::time_point<std::chrono::steady_clock> lastStabilize;
    std::chrono::time_point<std::chrono::steady_clock> lastFixFingers;
    std::chrono::time_point<std::chrono::steady_clock> lastCheckPredecessor;

    std::vector<TaskPair> tasks;
    Poco::Net::SocketAddress ownAddress;
    ConnectionManager& connectionManager;
    FingerTable& fingerTable;
};