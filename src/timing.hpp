#pragma once

#include <chrono>

class Timing {
   public:
    Timing();

    void update();

    std::chrono::steady_clock::time_point getStarted() const;

    std::chrono::steady_clock::time_point getLastUpdated() const;

    template <
        class result_t   = std::chrono::milliseconds,
        class clock_t    = std::chrono::steady_clock,
        class duration_t = std::chrono::milliseconds
    >
    static auto since(std::chrono::time_point<clock_t, duration_t> const& start)
    {
        return std::chrono::duration_cast<result_t>(clock_t::now() - start);
    }


   private:
    std::chrono::steady_clock::time_point started;
    std::chrono::steady_clock::time_point lastUpdated;
};