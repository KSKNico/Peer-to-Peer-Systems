#include "taskManager.hpp"

#include <cassert>

TaskManager::TaskManager(const Poco::Net::SocketAddress& ownAddress, ConnectionManager& connectionManager, FingerTable& fingerTable) : ownAddress(ownAddress), connectionManager(connectionManager), fingerTable(fingerTable) {}

void TaskManager::addTask(std::unique_ptr<Task> task) {
    assert(task->getState() == TaskState::UNINITIALIZED);
    tasks.push_back(std::move(task));
}

void TaskManager::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    for (auto& task : tasks) {
        task->processMessage(from, message);
    }
}

void TaskManager::launchPeriodicTasks() {
    if (fingerTable.getSuccessor() == ownAddress) {
        return;
    }

    if (std::chrono::system_clock::now() - lastStabilize > stabilizeInterval) {
        lastStabilize = std::chrono::system_clock::now();
        auto stabilizeTask = std::make_unique<StabilizeTask>(ownAddress, fingerTable, connectionManager);
        tasks.push_back(std::move(stabilizeTask));
    }

    if (std::chrono::system_clock::now() - lastFixFingers > fixFingersInterval) {
        lastFixFingers = std::chrono::system_clock::now();
        auto fixFingersTask = std::make_unique<FixFingersTask>(ownAddress, fingerTable, connectionManager);
        tasks.push_back(std::move(fixFingersTask));
    }

    if (std::chrono::system_clock::now() - lastCheckPredecessor > checkPredecessorInterval) {
        lastCheckPredecessor = std::chrono::system_clock::now();
        auto checkPredecessorTask = std::make_unique<CheckPredecessorTask>(ownAddress, fingerTable, connectionManager);
        tasks.push_back(std::move(checkPredecessorTask));
    }
}

void TaskManager::update() {
    launchPeriodicTasks();

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