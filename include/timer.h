#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
public:
    Timer();
    ~Timer();

    // Set and start the timer
    void start(double duration);

    // Pause the timer
    void pause();

    // Resume the timer
    void resume();

    // Get remaining time (in double)
    double remaining();

    // Reset the timer
    void reset();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_pauseTime;
    std::chrono::duration<double> m_duration{0};
    std::chrono::duration<double> m_elapsedTime{0};
    bool m_paused{false};
};


#endif