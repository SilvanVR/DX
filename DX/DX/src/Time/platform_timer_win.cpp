#include "platform_timer.h"

#include <Windows.h>

/**********************************************************************
    class: PlatformTimer (platform_timer_win.cpp)

    author: S. Hau
    date: September 9, 2017

    Windows implementation for precise time measurement.
    Because the platform dependant time measurement is generally more
    precise, it is wise to implement it for every platform.
**********************************************************************/

#ifdef _WIN32

    //----------------------------------------------------------------------
    PlatformTimer::PlatformTimer()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        m_tickFrequency = frequency.QuadPart;
        m_tickFrequencyInSeconds = 1.0 / m_tickFrequency;
    }

    //----------------------------------------------------------------------
    I64 PlatformTimer::getTicks()
    {
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);

        return I64(ticks.QuadPart);
    }

#endif // _WIN32
