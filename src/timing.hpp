#include <chrono>


class Timing {
    public:
    void updateIntervalMessageTime();
    bool intervalMessageTimePassed() const;

    void updateStabilizeMessageTime();
    bool stabilizeMessageTimePassed() const;

    void updateFindFingersMessageTime();
    bool findFingersMessageTimePassed() const;

    private:
    std::chrono::system_clock::time_point lastIntervalMessageTime;
    const std::chrono::milliseconds intervalMessageTimeDelta = std::chrono::milliseconds(1000);

    std::chrono::system_clock::time_point lastStabilizeMessageTime;
    const std::chrono::milliseconds stabilizeMessageTimeDelta = std::chrono::seconds(10);

    std::chrono::system_clock::time_point lastFindFingersMessageTime;
    const std::chrono::milliseconds findFingersMessageTimeDelta = std::chrono::seconds(10);
};