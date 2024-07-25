#pragma once

#include <memory>
#include <vector>

#include "task.hpp"

class TaskManager {
   public:
    TaskManager(ConnectionManager& connectionManager, FingerTable& fingerTable) : connectionManager(connectionManager), fingerTable(fingerTable) {}
    void addTask(std::unique_ptr<Task> task);
    void update();
    void removeCompletedTasks();

   private:
    std::vector<std::unique_ptr<Task>> tasks;
    ConnectionManager& connectionManager;
    FingerTable& fingerTable;
};