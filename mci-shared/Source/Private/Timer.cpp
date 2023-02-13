#include "MCI/Timer.h"

namespace MCI
{

    Timer::Timer() { Reset(); }

    void Timer::Reset()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    float Timer::GetElapsedSeconds() const
    {
        return GetElapsedMilliseconds() * 0.001f;
    }

    float Timer::GetElapsedMilliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f;
    }

}