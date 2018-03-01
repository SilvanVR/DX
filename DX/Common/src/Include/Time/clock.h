#pragma once
/**********************************************************************
    class: Clock (clock.h)
    
    author: S. Hau
    date: October 27, 2017

    Represents a clock, from which time can be queried and to which 
    functions can be attached.

    @Consideration:
      - No dynamic allocations for callbacks
      - Lerp callbacks
      - Add clocks behinds the scenes to a "ClockManager" ?
**********************************************************************/

#include "durations.h"
#include "Common/finite_range.hpp"

namespace Time {

    //**********************************************************************
    #define INFINITY_DURATION 0

    //**********************************************************************
    enum class ECallFrequency
    {
        ONCE,
        REPEAT
    };

    //**********************************************************************
    class Clock
    {
    public:
        Clock(Milliseconds duration = INFINITY_DURATION);
        ~Clock() = default;

        //----------------------------------------------------------------------
        Seconds     getDuration()       const { return m_curTime.getUpperBound(); }
        bool        ticksBackwards()    const { return (m_tickModifier < 0); }

        //----------------------------------------------------------------------
        // Change the tickrate of the clock. A negative number means the clock
        // runs backwards.
        // @Params:
        //  "modifier": The new modifier applied to the clock.
        //----------------------------------------------------------------------
        void        setTickModifier(F32 modifier) { m_tickModifier = modifier; }

        //----------------------------------------------------------------------
        // @Return: Time in seconds this clock currently has.
        //----------------------------------------------------------------------
        Seconds     getTime() const { return m_curTime.value(); }

        //----------------------------------------------------------------------
        // Attach a callback to this clock.
        // @Params:
        //  "func": The function to call.
        //  "ms": Time in milliseconds when to call the func. If the time exceeds
        //        the duration, the modulus operator is applied. If the time is 
        //        negative, it starts from the end and goes backwards.
        //  "freq": Determines if the function should be called once or repeated.
        //----------------------------------------------------------------------
        CallbackID  attachCallback(const std::function<void()>& func, Milliseconds ms, ECallFrequency freq = ECallFrequency::REPEAT );

        //----------------------------------------------------------------------
        // Remove a callback from this clock.
        //----------------------------------------------------------------------
        void        clearCallback(CallbackID id);

        //----------------------------------------------------------------------
        // !!!!! Call this function every frame / update !!!!!
        // Updates the clock and calls callback functions if required.
        //----------------------------------------------------------------------
        void        tick(Seconds delta);

    private:
        Common::FiniteRange<Milliseconds>   m_curTime;
        Milliseconds                        m_lastTime      = 0;
        F32                                 m_tickModifier  = 1.0f;

        struct AttachedCallback
        {
            std::function<void()>   callback;
            Milliseconds            time;
            CallbackID              id;
            ECallFrequency          freq;
        };

        // List of attached callbacks
        std::vector<AttachedCallback> m_attachedCallbacks;

        //----------------------------------------------------------------------
        void _CheckCallbacks();

        //----------------------------------------------------------------------
        Clock(const Clock& other)                 = delete;
        Clock& operator = (const Clock& other)    = delete;
        Clock(Clock&& other)                      = delete;
        Clock& operator = (Clock&& other)         = delete;
    };


} // end namespaces