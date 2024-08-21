#include <chrono>

class Timing {
   public:
    Timing();

    void update();

    std::chrono::system_clock::time_point getStarted() const;

    std::chrono::system_clock::time_point getLastUpdated() const;

   private:
    std::chrono::system_clock::time_point started;
    std::chrono::system_clock::time_point lastUpdated;
};