#pragma once

#include <memory>
#include <vector>

#include "task.hpp"

class TaskManager {
   public:
    TaskManager(ConnectionManager& connectionManager, FingerTable& fingerTable);
    void addTask(std::unique_ptr<Task> task);
    void update();
    void removeCompletedTasks();
    void processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message);

   private:
    std::vector<std::unique_ptr<Task>> tasks;
    ConnectionManager& connectionManager;
    FingerTable& fingerTable;
};