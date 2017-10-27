#pragma once
/**********************************************************************
    class: Clock (clock.h)
    
    author: S. Hau
    date: October 27, 2017

    Represents a clock which measures past time using the PlatformTimer.

    @Consideration:
      - Poll time only once per frame from the CPU and use that.
**********************************************************************/

namespace Core {


    class Clock
    {
    public:
        Clock(bool loop = false);
        ~Clock() = default;

        //----------------------------------------------------------------------
        // Delta time (in seconds) between two subsequent calls of this function.
        //----------------------------------------------------------------------
        F64 getDelta();

        //----------------------------------------------------------------------
        // @Return:
        //   Amount of ticks since this clock was created.
        //----------------------------------------------------------------------
        U64 getCurTicks();

        //----------------------------------------------------------------------
        // @Return:
        //   Time in seconds since this clock was created.
        //----------------------------------------------------------------------
        F64 getTime();

        // void addFunction(TimePoint);

    private:
        bool    m_loop;
        U64     m_startTicks;

        //----------------------------------------------------------------------
        Clock(const Clock& other)                 = delete;
        Clock& operator = (const Clock& other)    = delete;
        Clock(Clock&& other)                      = delete;
        Clock& operator = (Clock&& other)         = delete;
    };


}