#include "timing.hpp"

Timing::Timing() : started(std::chrono::steady_clock::now()), lastUpdated(std::chrono::steady_clock::now()) {}

void Timing::update() {
    lastUpdated = std::chrono::steady_clock::now();
}

std::chrono::steady_clock::time_point Timing::getStarted() const {
    return started;
}

std::chrono::steady_clock::time_point Timing::getLastUpdated() const {
    return lastUpdated;
}