#include "timers.h"
/**********************************************************************
    class: CallbackTimer (timers.cpp)

    author: S. Hau
    date: October 28, 2017
**********************************************************************/


namespace Core { namespace Time {


    //---------------------------------------------------------------------------
    CallbackTimer::CallbackTimer(const CallbackInfo& cbInfo, Milliseconds ms, bool loop)
        : m_callbackInfo( cbInfo ), m_duration( ms ), m_loop( loop )
    {}

    //---------------------------------------------------------------------------
    void CallbackTimer::update( Seconds delta )
    {
        m_elapsedTime += delta;

        if (m_elapsedTime > m_duration)
        {
            m_callbackInfo.callback();
            m_elapsedTime -= m_duration;

            if ( !m_loop )
                m_finished = true;
        }
    }



} } // end namespaces