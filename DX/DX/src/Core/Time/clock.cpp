#include "clock.h"
/**********************************************************************
    class: Clock (clock.cpp)
    
    author: S. Hau
    date: October 27, 2017

    @Considerations:
       - Sophisticated ID generator
**********************************************************************/

#include "Core/OS/PlatformTimer/platform_timer.h"
#include "locator.h"

namespace Core { namespace Time {

    //----------------------------------------------------------------------
    U64 NextID()
    {
        static U64 clockIDs = 0;
        clockIDs++;
        return clockIDs;
    }

    //----------------------------------------------------------------------
    Clock::Clock( Milliseconds duration )
        : m_startTicks( OS::PlatformTimer::getTicks() ), m_duration( duration )
    {
    }

    //----------------------------------------------------------------------
    Seconds Clock::_Update()
    {
        static U64 lastTicks = 0;

        m_curTicks = OS::PlatformTimer::getTicks() - m_startTicks;
        U64 deltaTicks = m_curTicks - lastTicks;
        lastTicks = m_curTicks;

        m_delta = OS::PlatformTimer::ticksToSeconds( deltaTicks );

        _UpdateTimer();

        return m_delta;
    }

    //----------------------------------------------------------------------
    Seconds Clock::getTime() const
    {
        return OS::PlatformTimer::ticksToSeconds( getCurTicks() );
    }

    //----------------------------------------------------------------------
    CallbackID Clock::setInterval( const std::function<void()>& func, Milliseconds ms )
    {
        return _AttachCallback( func, ms, true );
    }

    //----------------------------------------------------------------------
    CallbackID Clock::setTimeout( const std::function<void()>& func, Milliseconds ms )
    {
        return _AttachCallback( func, ms, false );
    }

    //----------------------------------------------------------------------
    //CallbackID  Clock::attachFunction( const std::function<void()>& func, F32 ms, ECallFrequency freq )
    //{
    //    CallbackInfo info;
    //    info.callback   = func;
    //    info.id         = NextID();

    //    bool loop = (freq == ECallFrequency::REPEAT);
    //    m_timers[info.id] = CallbackTimer( info, ms, loop );

    //    return info.id;
    //}

    //----------------------------------------------------------------------
    void Clock::clearCallback(CallbackID id)
    {
        if (id == INVALID_CALLBACK_ID)
            return;

        if (m_timers.count( id ) == 0)
        {
            WARN( "Clock::clearCallback(): Given CallbackID #" + TS( id ) + " does not exist." );
            return;
        }

        m_timers.erase( id );
    }

    //----------------------------------------------------------------------
    void Clock::_UpdateTimer()
    {
        m_idsToRemove.clear();

        // Update timers
        for (auto& pair : m_timers)
        {
            auto& timer = pair.second;
            timer.update( m_delta );

            if ( timer.isFinished() )
                m_idsToRemove.push_back( pair.first );
        }

        // Remove timer now
        for (auto id : m_idsToRemove)
            clearCallback( id );
    }

    //----------------------------------------------------------------------
    CallbackID Clock::_AttachCallback(const std::function<void()>& func, Milliseconds ms, bool loop)
    {
        CallbackInfo info;
        info.callback   = std::move( func );
        info.id         = NextID();

        m_timers[info.id] = std::move( CallbackTimer( info, ms, loop ) );

        return info.id;
    }

} } // end namespaces