#pragma once
/**********************************************************************
    class: CallbackTimer (timers.h)

    author: S. Hau
    date: October 28, 2017

    Represents a timer which will call a given function when the
    time's up. It can be looped if desired.
**********************************************************************/

#include "durations.h"

namespace Core { namespace Time {


    //**********************************************************************
    class CallbackTimer
    {
    public:
        CallbackTimer(const std::function<void()>& callback, Milliseconds duration, bool loop);
        CallbackTimer() = default;
        ~CallbackTimer() = default;

        //---------------------------------------------------------------------------
        // Update this clock. Calls the callback when time's up.
        //---------------------------------------------------------------------------
        void update(Seconds delta);

        //---------------------------------------------------------------------------
        bool isFinished() const { return m_finished; }

    private:
        std::function<void()>   m_callback;
        Seconds                 m_elapsedTime   = 0;
        Seconds                 m_duration      = 0;
        bool                    m_loop          = true;
        bool                    m_finished      = false;
    };




} } // end namespaces