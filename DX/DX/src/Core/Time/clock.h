#pragma once
/**********************************************************************
    class: Clock (clock.h)
    
    author: S. Hau
    date: October 27, 2017

    Represents a clock, from which time can be queried and to which 
    functions can be attached.

    @Consideration:
      - Scale time
      - No dynamic allocations for callbacks
      - Lerp callbacks
      - Add clocks behinds the scenes to a "ClockManager" and update all their
**********************************************************************/

#include "durations.h"

namespace Core { namespace Time {

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
        // @Return: Delta time (in seconds) between two ticks of this clock.
        //----------------------------------------------------------------------
        Seconds getDelta() const { return m_delta; }

        //----------------------------------------------------------------------
        // @Return: Time in seconds since this clock was created.
        //----------------------------------------------------------------------
        Seconds getTime() const { return m_curTime; }

        //----------------------------------------------------------------------
        // Attach a function to this clock.
        // @Params:
        //  "func": The function to call.
        //  "ms": Time in milliseconds when to call the func. If the time exceeds
        //        the duration, the modulus operator is applied.
        //  "freq": Determines if the function should be called once or repeated.
        //----------------------------------------------------------------------
        CallbackID  attachCallback(const std::function<void()>& func, Milliseconds ms, ECallFrequency freq = ECallFrequency::REPEAT );

        //----------------------------------------------------------------------
        // Remove a callback from this clock.
        //----------------------------------------------------------------------
        void        clearCallback(CallbackID id);

        //----------------------------------------------------------------------
        // !!!!! Call this function every frame / update !!!!!
        // Updates the clock, returns the newly calculated delta and calls
        // callback functions if required.
        //----------------------------------------------------------------------
        Seconds _Update();

    private:
        U64             m_lastTicks     = 0;
        Milliseconds    m_delta         = 0;
        Milliseconds    m_curTime       = 0;
        Milliseconds    m_lastTime      = 0;
        Milliseconds    m_duration      = 0;

        struct AttachedCallback
        {
            CallbackID              id;
            std::function<void()>   callback;
            ECallFrequency          freq;
            Milliseconds            time;
        };

        // List of attached functions
        std::vector<AttachedCallback> m_attachedCallbacks;

        //----------------------------------------------------------------------
        void _CheckCallbacks();

        //----------------------------------------------------------------------
        Clock(const Clock& other)                 = delete;
        Clock& operator = (const Clock& other)    = delete;
        Clock(Clock&& other)                      = delete;
        Clock& operator = (Clock&& other)         = delete;
    };


    //----------------------------------------------------------------------
    // Check if a given value is between the two boundaries b1 and b2.
    // @Return:
    //   True if [b1 <= val <= b2]
    //----------------------------------------------------------------------
    template <class T>
    bool isBetween(T b1, T val, T b2)
    {
        return ( ( val >= b1 ) && ( val <= b2 ) );
    }

} } // end namespaces