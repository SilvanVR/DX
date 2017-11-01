#pragma once
/**********************************************************************
    class: Clock (clock.h)
    
    author: S. Hau
    date: October 27, 2017

    Represents a clock which measures past time using the PlatformTimer.

    @Consideration:
      - Scale time
      - No dynamic allocations for callbacks
      - attach functions to specific points in time (sync timers with clock)
      - Lerp callbacks
**********************************************************************/

#include "timers.h"


namespace Core { namespace Time {

    //**********************************************************************
    #define DURATION_INFINITY 0

    //**********************************************************************
    enum class ECallFrequency
    {
        CALL_ONCE,
        REPEAT
    };

    //**********************************************************************
    class Clock
    {
    public:
        Clock(Milliseconds duration = DURATION_INFINITY);
        ~Clock() = default;

        //----------------------------------------------------------------------
        // @Return: Delta time (in seconds) between two frames.
        //----------------------------------------------------------------------
        Seconds getDelta() const { return m_delta; }

        //----------------------------------------------------------------------
        // @Return: Time in seconds since this clock was created.
        //----------------------------------------------------------------------
        Seconds getTime() const;

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
        // Attach a function to this clock.
        // @Params:
        //  "func": The function to call.
        //  "ms": Time in milliseconds when to call the func. If the time exceeds
        //        the duration, the modulus operator is applied.
        //  "freq": Determines if the function should be called once or repeated.
        //----------------------------------------------------------------------
       // CallbackID  attachFunction(const std::function<void()>& func, F32 ms, ECallFrequency freq = ECallFrequency::REPEAT );

        //----------------------------------------------------------------------
        // !!!!! Call this function every frame / update !!!!!
        // Updates the clock, returns the newly calculated delta and calls
        // callback functions if required.
        //----------------------------------------------------------------------
        Seconds _Update();

    private:
        Seconds         m_delta         = 0;
        U64             m_startTicks    = 0;
        U64             m_curTicks      = 0;
        Milliseconds    m_duration      = 0;

        std::vector<CallbackID>             m_idsToRemove;
        std::map<CallbackID, CallbackTimer> m_timers;

        //----------------------------------------------------------------------
        // Used for other clocks to poll the cpu time.
        // @Return:
        //   Amount of ticks since this clock was created.
        //----------------------------------------------------------------------
        U64 getCurTicks() const { return m_curTicks; }

        //----------------------------------------------------------------------
        CallbackID  _AttachCallback(const std::function<void()>& func, Milliseconds ms, bool loop);
        void        _UpdateTimer();

        //----------------------------------------------------------------------
        Clock(const Clock& other)                 = delete;
        Clock& operator = (const Clock& other)    = delete;
        Clock(Clock&& other)                      = delete;
        Clock& operator = (Clock&& other)         = delete;
    };


} } // end namespaces