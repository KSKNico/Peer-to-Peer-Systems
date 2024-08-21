#include "timing.hpp"

Timing::Timing() : started(std::chrono::system_clock::now()), lastUpdated(std::chrono::system_clock::now()) {}

void Timing::update() {
    lastUpdated = std::chrono::system_clock::now();
}

std::chrono::system_clock::time_point Timing::getStarted() const {
    return started;
}

std::chrono::system_clock::time_point Timing::getLastUpdated() const {
    return lastUpdated;
}