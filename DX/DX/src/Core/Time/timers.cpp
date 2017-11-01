#include "timers.h"
/**********************************************************************
    class: CallbackTimer (timers.cpp)

    author: S. Hau
    date: October 28, 2017
**********************************************************************/


namespace Core { namespace Time {


    //---------------------------------------------------------------------------
    CallbackTimer::CallbackTimer( const std::function<void()>& callback, Milliseconds duration, bool loop )
        : m_callback( callback ), m_duration( duration ), m_loop( loop )
    {}

    //---------------------------------------------------------------------------
    void CallbackTimer::update( Seconds delta )
    {
        m_elapsedTime += delta;

        if (m_elapsedTime > m_duration)
        {
            m_callback();
            m_elapsedTime -= m_duration;

            if ( !m_loop )
                m_finished = true;
        }
    }



} } // end namespaces