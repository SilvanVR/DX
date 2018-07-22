#pragma once
/**********************************************************************
    class: MasterClock (master_clock.h)

    author: S. Hau
    date: November 01, 2017

    Represents a clock which measures past time using the PlatformTimer.
    The Master-Clock serves as a Singleton, who polls the cpu-time once
    per frame. Every other clock will use that.

    Besides, the master clock has unique functionalities:
      setInterval(): Attach a callback function, which will be called
                     periodically.
      setTimeout():  Attach a callback function, which will be called
                     once after the specified amount of time.

    Why std::function instead of raw function pointer? 
     - Simple reason: std::function can capture states, whereas raw
       function pointers can not. I think that justifies the overhead.

    @Consideration:
      - No dynamic allocations for callbacks 
        (Isn't that bad because it happens rarely)
**********************************************************************/

#include "timers.h"

namespace Time {

    //**********************************************************************
    class MasterClock
    {
    public:
        MasterClock();
        ~MasterClock() = default;

        //----------------------------------------------------------------------
        // @Return: Delta time (in seconds) between two frames.
        //----------------------------------------------------------------------
        Seconds     getDelta() const { return m_delta; }

        //----------------------------------------------------------------------
        // @Return: Time in seconds since this clock was created.
        //----------------------------------------------------------------------
        Seconds     getTime() const;

        //----------------------------------------------------------------------
        // Attach a function to this clock.
        // @Params:
        //  "func": The function to call.
        //  "ms": Time in milliseconds between each function call.
        //----------------------------------------------------------------------
        CallbackID  setInterval(const std::function<void()>& func, Milliseconds ms);

        //----------------------------------------------------------------------
        // Attach a function to this clock.
        // @Params:
        //  "func": The function to call once.
        //  "ms": Time in milliseconds after the function will be called.
        //----------------------------------------------------------------------
        CallbackID  setTimeout(const std::function<void()>& func, Milliseconds ms);

        //----------------------------------------------------------------------
        // Remove a callback from this clock.
        //----------------------------------------------------------------------
        void        clearCallback(CallbackID id);

        //----------------------------------------------------------------------
        // Remove all callbacks from this clock.
        //----------------------------------------------------------------------
        void        clearAllCallbacks(){ m_timers.clear(); }

        //----------------------------------------------------------------------
        // !!!!! Call this function every frame !!!!!
        // Updates the clock, returns the newly calculated delta and calls
        // callback functions if required.
        //----------------------------------------------------------------------
        Seconds     _Update();

    private:
        Seconds m_delta         = 0;
        U64     m_startTicks    = 0;
        U64     m_curTicks      = 0;
        U64     m_lastTicks     = 0;

        HashMap<CallbackID, CallbackTimer> m_timers;

        //----------------------------------------------------------------------
        // Used for other clocks to poll the cpu time.
        // @Return:
        //   Amount of ticks since this clock was created.
        //----------------------------------------------------------------------
        //friend class Clock;
        //static MasterClock* s_Instance;
        //static U64 getCurTicks() { return s_Instance->m_curTicks; }

        //----------------------------------------------------------------------
        CallbackID  _AttachCallback(const std::function<void()>& func, Milliseconds ms, bool loop);
        void        _UpdateTimer();

        //----------------------------------------------------------------------
        MasterClock(const MasterClock& other)                 = delete;
        MasterClock& operator = (const MasterClock& other)    = delete;
        MasterClock(MasterClock&& other)                      = delete;
        MasterClock& operator = (MasterClock&& other)         = delete;
    };


} // end namespaces