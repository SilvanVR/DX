#pragma once
/**********************************************************************
    class: Timer + CallbackTimer (timers.h)
    
    author: S. Hau
    date: October 28, 2017
**********************************************************************/


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
        CallbackTimer() = default;
        ~CallbackTimer() = default;
        CallbackTimer(const CallbackInfo& cbInfo, F32 durationInMs, bool loop);

        //---------------------------------------------------------------------------
        // Update this clock. Calls the callback when necessary.
        //---------------------------------------------------------------------------
        void            update(F64 delta);

        //---------------------------------------------------------------------------
        CallbackID      getID()         const { return m_callbackInfo.id; }
        bool            isFinished()    const { return m_finished; }

    private:
        CallbackInfo    m_callbackInfo;
        F64             m_elapsedTime   = 0;
        F64             m_duration      = 0;
        bool            m_loop          = true;
        bool            m_finished      = false;
    };




} } // end namespaces