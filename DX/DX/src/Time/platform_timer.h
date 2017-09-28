#pragma once

/**********************************************************************
    class: PlatformTimer (platform_timer.h)

    author: S. Hau
    date: September 9, 2017

    Specifies the interface for platform independant time queries.
**********************************************************************/

#include "Interfaces/singleton.hpp"
#include "system_time.hpp"

class PlatformTimer : public Singleton<PlatformTimer>
{
public:
    PlatformTimer();
    ~PlatformTimer() {}

    //**********************************************************************
    // Returns the current time of the pc.
    // @return: Struct which contains the time like year, month, day etc.
    //**********************************************************************
    SystemTime getCurrentTime() const;

    //**********************************************************************
    // Returns the number of ticks since the pc was started.
    // Query how much a tick is in time with getTickFrequencyInSeconds()
    // @return: Amount of ticks since the pc was started.
    //**********************************************************************
    I64 getTicks() const;

    //----------------------------------------------------------------------
    // Returns the frequency of the used clock.
    // (1.0 / frequency) delievers the interval in seconds.
    //----------------------------------------------------------------------
    inline I64 getTickFrequency() const { return m_tickFrequency; }

    //----------------------------------------------------------------------
    // Returns the frequency of the used clock in seconds.
    // Multiply the amount of ticks with this to get the final passed
    // time in seconds or use the function ticksToSeconds(...).
    //----------------------------------------------------------------------
    inline F64 getTickFrequencyInSeconds() const { return m_tickFrequencyInSeconds; }

    //----------------------------------------------------------------------
    // Converts amount of ticks to passed time in different formats.
    //----------------------------------------------------------------------
    inline F64 ticksToSeconds(I64 ticks) const { return ticks * getTickFrequencyInSeconds(); }
    inline F64 ticksToMilliSeconds(I64 ticks) const { return ticksToSeconds(ticks) * 1000.0; }
    inline F64 ticksToMicroSeconds(I64 ticks) const { return ticksToMilliSeconds(ticks) * 1000.0; }
    inline F64 ticksToNanoSeconds(I64 ticks) const { return ticksToMicroSeconds(ticks) * 1000.0; }

private:
    I64 m_tickFrequency = 0;
    F64 m_tickFrequencyInSeconds = 0.0;

    // Forbid copy + copy assignment and rvalue copying
    PlatformTimer(const PlatformTimer& other) = delete;
    PlatformTimer& operator = (const PlatformTimer& other) = delete;
    PlatformTimer(const PlatformTimer&& other) = delete;
    PlatformTimer& operator = (const PlatformTimer&& other) = delete;
};