#include "timer.h"

Timer::Timer() {}

Timer::~Timer()
{
    m_paused = true;
    m_duration = std::chrono::duration<double>(0.0);
}

void Timer::start(double duration)
{
    m_startTime = std::chrono::high_resolution_clock::now();
    m_duration = std::chrono::duration<double>(duration);
    m_paused = false;
}

void Timer::pause()
{
    if (!m_paused)
    {
        m_elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - m_startTime);
        m_paused = true;
    }
}

void Timer::resume()
{
    if (m_paused)
    {
        auto now = std::chrono::high_resolution_clock::now();
        m_startTime += now - m_pauseTime;
        m_paused = false;
    }
}

double Timer::remaining()
{
    if (m_paused)
    {
        return m_duration.count() - m_elapsedTime.count();
    }
    else
    {
        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - m_startTime;
        return m_duration.count() - elapsed.count();
    }
}

void Timer::reset() {
    m_paused = true;
    m_duration = std::chrono::duration<double>(180);
    m_startTime = decltype(m_startTime){};
}

