#include "clock.h"
/**********************************************************************
    class: Clock (clock.cpp)
    
    author: S. Hau
    date: October 27, 2017

    @Considerations:
       - Sophisticated ID generator
       - U64 for time measurements?
       - Better data structure for callback-functions
         + sorted linked-list and check only next elem? (Save index where you were at last frame)
**********************************************************************/

#include "locator.h"
#include "master_clock.h"
#include "Core/OS/PlatformTimer/platform_timer.h"

namespace Core { namespace Time {


    //----------------------------------------------------------------------
    static U64 NextID()
    {
        static U64 callbackIDs = 0;
        callbackIDs++;
        return callbackIDs;
    }

    //----------------------------------------------------------------------
    Clock::Clock( Milliseconds duration )
        : m_duration( duration )
    {
    }

    //----------------------------------------------------------------------
    Seconds Clock::_Update()
    {
        U64 m_curTicks = MasterClock::getCurTicks();
        U64 deltaTicks = m_curTicks - m_lastTicks;
        m_lastTicks = m_curTicks;

        m_delta = OS::PlatformTimer::ticksToMilliSeconds( deltaTicks );

        m_lastTime = m_curTime;
        m_curTime += m_delta;
        if (m_curTime > m_duration)
            m_curTime -= m_duration;

        _CheckCallbacks();

        return m_delta;
    }

    //----------------------------------------------------------------------
    CallbackID  Clock::attachCallback( const std::function<void()>& func, Milliseconds ms, ECallFrequency freq )
    {
        AttachedCallback newFunc;
        newFunc.callback   = func;
        newFunc.id         = NextID();
        newFunc.freq       = freq;
        newFunc.time       = (ms % m_duration);

        m_attachedCallbacks.push_back( newFunc );

        return newFunc.id;
    }

    //----------------------------------------------------------------------
    void Clock::clearCallback(CallbackID id)
    {
        if (id == INVALID_CALLBACK_ID)
            return;

        for (auto it = m_attachedCallbacks.begin(); it != m_attachedCallbacks.end(); it++)
        {
            if (it->id == id)
            {
                m_attachedCallbacks.erase(it);
                return;
            }
        }

        WARN( "Clock::clearCallback(): Given CallbackID #" + TS( id ) + " does not exist." );
    }

    //----------------------------------------------------------------------
    void Clock::_CheckCallbacks()
    {
        for (auto it = m_attachedCallbacks.begin(); it != m_attachedCallbacks.end(); )
        {
            if ( isBetween( m_lastTime, it->time, m_curTime ) )
            {
                it->callback();
                switch ( it->freq )
                {
                    case ECallFrequency::ONCE:
                    {
                        it = m_attachedCallbacks.erase( it );
                        continue;
                    }
                    case ECallFrequency::REPEAT:
                        // do nothing
                        break;
                }
            }
            it++;
        }
    }


    //----------------------------------------------------------------------
    //void Clock::_AddFunction(const AttachedCallback& func)
    //{
    //    bool foundSlot = false;
    //    for (auto it = m_attachedFunctions.begin(); it != m_attachedFunctions.end(); it++)
    //    {
    //        if (func.time > m_curTime && func.time < it->time)
    //        {
    //            m_attachedFunctions.insert( it, func );
    //            foundSlot = true;
    //            break;
    //        }
    //    }
    //    if (!foundSlot)
    //        m_attachedFunctions.push_back( func );
    //}

} } // end namespaces