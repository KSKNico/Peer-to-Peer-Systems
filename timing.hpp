#include <chrono>


class Timing {
    public:
    void updateIntervalMessageTime();
    bool intervalMessageTimePassed();

    private:
    std::chrono::system_clock::time_point lastIntervalMessageTime;
    const std::chrono::milliseconds intervalMessageTimeDelta = std::chrono::milliseconds(1000);
};