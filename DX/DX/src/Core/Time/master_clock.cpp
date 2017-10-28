#include "master_clock.h"
/**********************************************************************
    class: MasterClock (master_clock.cpp)
    
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
    MasterClock::MasterClock()
        : m_startTicks( OS::PlatformTimer::getTicks() )
    {
    }

    //----------------------------------------------------------------------
    F64 MasterClock::_Update()
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
    F64 MasterClock::getTime()
    {
        return OS::PlatformTimer::ticksToSeconds( getCurTicks() );
    }

    //----------------------------------------------------------------------
    CallbackID MasterClock::setInterval( const std::function<void()>& func, F32 ms )
    {
        return _AttachCallback( func, ms, true );
    }

    //----------------------------------------------------------------------
    CallbackID MasterClock::setTimeout( const std::function<void()>& func, F32 ms )
    {
        return _AttachCallback( func, ms, false );
    }

    //----------------------------------------------------------------------
    void MasterClock::clearCallback(CallbackID id)
    {
        if (id == INVALID_CALLBACK_ID)
            return;

        if (m_timers.count( id ) == 0)
        {
            WARN( "MasterClock::clearCallback(): Given CallbackID #" + TS( id ) + " does not exist." );
            return;
        }

        m_timers.erase( id );
    }

    //----------------------------------------------------------------------
    void MasterClock::_UpdateTimer()
    {
        idsToRemove.clear();

        // Update timers
        for (auto& pair : m_timers)
        {
            auto& timer = pair.second;
            timer.update( m_delta );

            if ( timer.isFinished() )
                idsToRemove.push_back( pair.first );
        }

        // Remove timer now
        for (auto id : idsToRemove)
            clearCallback( id );
    }

    //----------------------------------------------------------------------
    CallbackID MasterClock::_AttachCallback(const std::function<void()>& func, F32 ms, bool loop)
    {
        CallbackInfo info;
        info.callback   = std::move( func );
        info.id         = NextID();

        m_timers[info.id] = std::move( CallbackTimer( info, ms, loop ) );

        return info.id;
    }

} } // end namespaces