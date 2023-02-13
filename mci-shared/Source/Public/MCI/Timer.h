#pragma once

#include <chrono>

namespace MCI
{

    class Timer
    {
    public:

        Timer();

        void Reset();

        float GetElapsedSeconds() const;

        float GetElapsedMilliseconds() const;

    private:

        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

    };

}