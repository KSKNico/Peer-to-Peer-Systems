#include "timing.hpp"

void Timing::updateIntervalMessageTime() {
    lastIntervalMessageTime = std::chrono::system_clock::now();
}

bool Timing::intervalMessageTimePassed() const {
    auto now = std::chrono::system_clock::now();
    return now - lastIntervalMessageTime > intervalMessageTimeDelta;
}

void Timing::updateStabilizeMessageTime() {
    lastStabilizeMessageTime = std::chrono::system_clock::now();
}

bool Timing::stabilizeMessageTimePassed() const {
    auto now = std::chrono::system_clock::now();
    return now - lastStabilizeMessageTime > stabilizeMessageTimeDelta;
}

void Timing::updateFindFingersMessageTime() {
    lastFindFingersMessageTime = std::chrono::system_clock::now();
}

bool Timing::findFingersMessageTimePassed() const {
    auto now = std::chrono::system_clock::now();
    return now - lastFindFingersMessageTime > findFingersMessageTimeDelta;
}