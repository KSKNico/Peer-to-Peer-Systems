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

template <
    class result_t   = std::chrono::milliseconds,
    class clock_t    = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds
>
static auto since(std::chrono::time_point<clock_t, duration_t> const& start)
    {
        return std::chrono::duration_cast<result_t>(clock_t::now() - start);
    }