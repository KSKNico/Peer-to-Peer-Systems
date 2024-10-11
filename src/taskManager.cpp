#include "taskManager.hpp"

#include <cassert>

#include "spdlog/spdlog.h"

TaskManager::TaskManager(const Poco::Net::SocketAddress& ownAddress, ConnectionManager& connectionManager, FingerTable& fingerTable) : ownAddress(ownAddress), connectionManager(connectionManager), fingerTable(fingerTable) {}

void TaskManager::addTask(std::unique_ptr<Task>&& task) {
    assert(task->getState() == TaskState::UNINITIALIZED);
    spdlog::get(ownAddress.toString())->debug("Adding task");
    tasks.push_back({std::move(task), Timing()});
}

void TaskManager::processMessage(const Poco::Net::SocketAddress& from, const std::unique_ptr<Message>& message) {
    for (auto& [task, timing] : tasks) {
        if (task->processMessage(from, message)) {
            timing.update();
        }
    }
}

void TaskManager::launchPeriodicTasks() {
    if (fingerTable.getSuccessor() == ownAddress) {
        return;
    }

    if (Timing::since(lastStabilize) > stabilizeInterval) {
        lastStabilize = std::chrono::steady_clock::now();
        auto stabilizeTask = std::make_unique<StabilizeTask>(fingerTable, connectionManager, ownAddress);
        addTask(std::move(stabilizeTask));
    }

    if (Timing::since(lastFixFingers) > fixFingersInterval) {
        lastFixFingers = std::chrono::steady_clock::now();
        auto fixFingersTask = std::make_unique<FixFingersTask>(fingerTable, connectionManager, ownAddress);
        addTask(std::move(fixFingersTask));
    }

    if (Timing::since(lastCheckPredecessor) > checkPredecessorInterval) {
        lastCheckPredecessor = std::chrono::steady_clock::now();
        auto checkPredecessorTask = std::make_unique<CheckPredecessorTask>(fingerTable, connectionManager, ownAddress);
        addTask(std::move(checkPredecessorTask));
    }
}

void TaskManager::update() {
    launchPeriodicTasks();

    auto it = tasks.begin();
    while (it != tasks.end()) {
        auto& [task, timing] = *it;

        // this erases only running tasks that have timed out
        if (Timing::since(it->second.getLastUpdated()) > taskTimeout &&
            task->getState() == TaskState::RUNNING) {
            it = tasks.erase(it);
            spdlog::get(ownAddress.toString())->warn("Task timed out");
        } else {
            task->init();
            task->update();
            ++it;
        }
    }
}

/*
void TaskManager::removeCompletedTasks() {
    tasks.erase(std::remove_if(tasks.begin(),
                               tasks.end(),
                               [](const std::pair<const std::unique_ptr<Task>, Timing>& pair) {
                                   return pair.first->getState() == TaskState::FINISHED;
                               }),
                tasks.end());
}
*/