#include "platform_timer.h"

#include <chrono>
#include <ctime>

/**********************************************************************
    class: PlatformTimer (platform_timer_chrono.cpp)

    author: S. Hau
    date: September 9, 2017

    Platform INDEPENDANT time measurement using the c++11 chrono lib.
    Requires C++11. Used as a fallback if plattform specific time
    measurement is not implemented.
**********************************************************************/


#ifndef _WIN32

    using Clock = std::chrono::high_resolution_clock;

    //----------------------------------------------------------------------
    PlatformTimer::PlatformTimer()
    {
        // Period represents the ratio of the frequency: period = num / den
        m_tickFrequency = Clock::period::den;
        m_tickFrequencyInSeconds = 1.0 / m_tickFrequency;
    }

    //----------------------------------------------------------------------
    I64 PlatformTimer::getTicks()
    {
        return I64( Clock::now().time_since_epoch().count() );
    }

    //----------------------------------------------------------------------
    SystemTime PlatformTimer::getCurrentTime()
    {
        using SystemClock = std::chrono::system_clock;

        auto now = SystemClock::now();
        std::time_t time = SystemClock::to_time_t( now );

        struct tm timeInfo = {};
        localtime_s( &timeInfo, &time );

        SystemTime st = {};
        st.year         = timeInfo.tm_year + 1900;
        st.month        = timeInfo.tm_mon + 1;
        st.dayOfWeek    = timeInfo.tm_wday;
        st.day          = timeInfo.tm_mday;
        st.hour         = timeInfo.tm_hour;
        st.minute       = timeInfo.tm_min;
        st.second       = timeInfo.tm_sec;
        st.milliseconds = 0;

        return st;
    }


#endif // !_WIN32
