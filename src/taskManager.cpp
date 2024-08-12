#include "taskManager.hpp"

#include <cassert>

TaskManager::TaskManager(ConnectionManager& connectionManager, FingerTable& fingerTable) : connectionManager(connectionManager), fingerTable(fingerTable) {}

void TaskManager::addTask(std::unique_ptr<Task> task) {
    assert(task->getState() == TaskState::UNINITIALIZED);
    tasks.push_back(std::move(task));
}

void TaskManager::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    for (auto& task : tasks) {
        task->processMessage(from, message);
    }
}

void TaskManager::update() {
    for (auto& task : tasks) {
        task->init();
        task->update();
    }
}

void TaskManager::removeCompletedTasks() {
    tasks.erase(std::remove_if(tasks.begin(),
                               tasks.end(),
                               [](const std::unique_ptr<Task>& task) {
                                   return task->getState() == TaskState::FINISHED;
                               }),
                tasks.end());
}