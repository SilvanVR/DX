#include "timers.h"
/**********************************************************************
    class: CallbackTimer (timers.cpp)

    author: S. Hau
    date: October 28, 2017
**********************************************************************/


namespace Core { namespace Time {


    //---------------------------------------------------------------------------
    CallbackTimer::CallbackTimer(const CallbackInfo& cbInfo, F32 durationInMs, bool loop)
        : m_callbackInfo( cbInfo ), m_duration( durationInMs * 0.001 ), m_loop( loop )
    {}

    //---------------------------------------------------------------------------
    void CallbackTimer::update( F64 delta )
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