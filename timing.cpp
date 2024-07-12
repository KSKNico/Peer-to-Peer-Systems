#include "timing.hpp"

void Timing::updateIntervalMessageTime() {
    lastIntervalMessageTime = std::chrono::system_clock::now();
}

bool Timing::intervalMessageTimePassed() {
    auto now = std::chrono::system_clock::now();
    return now - lastIntervalMessageTime > intervalMessageTimeDelta;
}