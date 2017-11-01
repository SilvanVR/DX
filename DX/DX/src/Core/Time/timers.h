#pragma once
/**********************************************************************
    class: CallbackTimer (timers.h)

    author: S. Hau
    date: October 28, 2017
**********************************************************************/

#include "durations.h"

namespace Core { namespace Time {

    //---------------------------------------------------------------------------
    struct CallbackInfo
    {
        CallbackID id;
        std::function<void()> callback;
    };

    //**********************************************************************
    class CallbackTimer
    {
    public:
        CallbackTimer(const CallbackInfo& cbInfo, Milliseconds ms, bool loop);
        CallbackTimer() = default;
        ~CallbackTimer() = default;

        //---------------------------------------------------------------------------
        // Update this clock. Calls the callback when necessary.
        //---------------------------------------------------------------------------
        void            update(Seconds delta);

        //---------------------------------------------------------------------------
        CallbackID      getID()         const { return m_callbackInfo.id; }
        bool            isFinished()    const { return m_finished; }

    private:
        CallbackInfo    m_callbackInfo;
        Seconds         m_elapsedTime   = 0;
        Seconds         m_duration      = 0;
        bool            m_loop          = true;
        bool            m_finished      = false;
    };




} } // end namespaces