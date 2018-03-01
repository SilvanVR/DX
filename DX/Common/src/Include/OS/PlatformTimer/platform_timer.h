#pragma once

/**********************************************************************
    class: PlatformTimer (platform_timer.h)

    author: S. Hau
    date: September 28, 2017

    Specifies the interface for platform independant time queries.
    Access via static methods.
**********************************************************************/

#include "system_time.hpp"

namespace OS {

    class PlatformTimer
    {
        // Private constructor. Only one static instance allowed for initialization.
        PlatformTimer() { _Init(); } 

    public:
        //----------------------------------------------------------------------
        // Returns the current time of the pc.
        // @Return: 
        //   Struct which contains the time like year, month, day etc.
        //----------------------------------------------------------------------
        static SystemTime getCurrentTime();

        //----------------------------------------------------------------------
        // Returns the number of ticks since the pc was started.
        // Query how much a tick is in time with getTickFrequencyInSeconds()
        // @Return: 
        //   Amount of ticks since the pc was started.
        //----------------------------------------------------------------------
        static U64 getTicks();

        //----------------------------------------------------------------------
        // Returns the frequency of the used clock.
        // (1.0 / frequency) delievers the interval in seconds.
        //----------------------------------------------------------------------
        static inline U64 getTickFrequency() { return m_tickFrequency; }

        //----------------------------------------------------------------------
        // Returns the frequency of the used clock in seconds.
        // Multiply the amount of ticks with this to get the final passed
        // time in seconds or use the function ticksToSeconds(...).
        //----------------------------------------------------------------------
        static inline F64 getTickFrequencyInSeconds() { return m_tickFrequencyInSeconds; }

        //----------------------------------------------------------------------
        // Converts amount of ticks to passed time in different formats.
        //----------------------------------------------------------------------
        static inline F64 ticksToSeconds(U64 ticks) { return ticks * getTickFrequencyInSeconds(); }
        static inline F64 ticksToMilliSeconds(U64 ticks) { return ticksToSeconds(ticks) * 1000.0; }
        static inline F64 ticksToMicroSeconds(U64 ticks) { return ticksToMilliSeconds(ticks) * 1000.0; }
        static inline F64 ticksToNanoSeconds(U64 ticks) { return ticksToMicroSeconds(ticks) * 1000.0; }

    private:
        static PlatformTimer    m_instance;
        static U64              m_tickFrequency;
        static F64              m_tickFrequencyInSeconds;

        void _Init();

        // Forbid copy + copy assignment and rvalue copying
        PlatformTimer (const PlatformTimer& other)              = delete;
        PlatformTimer& operator= (const PlatformTimer& other)   = delete;
        PlatformTimer (PlatformTimer&& other)                   = delete;
        PlatformTimer& operator= (PlatformTimer&& other)        = delete;
    };

}