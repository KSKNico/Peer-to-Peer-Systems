#include "taskManager.hpp"

#include <cassert>

TaskManager::TaskManager(const Poco::Net::SocketAddress& ownAddress, ConnectionManager& connectionManager, FingerTable& fingerTable) : ownAddress(ownAddress), connectionManager(connectionManager), fingerTable(fingerTable) {}

void TaskManager::addTask(std::unique_ptr<Task>&& task) {
    assert(task->getState() == TaskState::UNINITIALIZED);
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

    if (std::chrono::system_clock::now() - lastStabilize > stabilizeInterval) {
        lastStabilize = std::chrono::system_clock::now();
        auto stabilizeTask = std::make_unique<StabilizeTask>(ownAddress, fingerTable, connectionManager);
        addTask(std::move(stabilizeTask));
    }

    if (std::chrono::system_clock::now() - lastFixFingers > fixFingersInterval) {
        lastFixFingers = std::chrono::system_clock::now();
        auto fixFingersTask = std::make_unique<FixFingersTask>(ownAddress, fingerTable, connectionManager);
        addTask(std::move(fixFingersTask));
    }

    if (std::chrono::system_clock::now() - lastCheckPredecessor > checkPredecessorInterval) {
        lastCheckPredecessor = std::chrono::system_clock::now();
        auto checkPredecessorTask = std::make_unique<CheckPredecessorTask>(ownAddress, fingerTable, connectionManager);
        addTask(std::move(checkPredecessorTask));
    }
}

void TaskManager::update() {
    launchPeriodicTasks();

    auto it = tasks.begin();
    while (it != tasks.end()) {
        auto& [task, timing] = *it;
        if (task->getState() == TaskState::UNINITIALIZED) {
            continue;
        }

        if (it->second.getLastUpdated() - std::chrono::system_clock::now() > taskTimeout) {
            tasks.erase(it);
        } else {
            task->init();
            task->update();
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