#include "taskManager.hpp"

TaskManager::TaskManager(ConnectionManager& connectionManager, FingerTable& fingerTable) : connectionManager(connectionManager), fingerTable(fingerTable) {}

void TaskManager::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    for (auto& task : tasks) {
        task->processMessage(from, message);
    }
}

void TaskManager::update() {
    for (auto& task : tasks) {
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