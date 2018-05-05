#include "master_clock.h"
/**********************************************************************
    class: MasterClock (master_clock.cpp)

    author: S. Hau
    date: November 01, 2017

    @Considerations:
       - Sophisticated ID generator
**********************************************************************/

#include "OS/PlatformTimer/platform_timer.h"
#include "Logging/logging.h"

namespace Time {

    //----------------------------------------------------------------------
    static U64 NextID()
    {
        static U64 callbackIDs = 0;
        callbackIDs++;
        return callbackIDs;
    }

    //----------------------------------------------------------------------
    MasterClock::MasterClock()
        : m_startTicks( OS::PlatformTimer::getTicks() )
    {}

    //----------------------------------------------------------------------
    Seconds MasterClock::_Update()
    {
        m_curTicks = OS::PlatformTimer::getTicks() - m_startTicks;
        U64 deltaTicks = m_curTicks - m_lastTicks;
        m_lastTicks = m_curTicks;

        m_delta = OS::PlatformTimer::ticksToSeconds( deltaTicks );

        _UpdateTimer();

        return m_delta;
    }

    //----------------------------------------------------------------------
    Seconds MasterClock::getTime() const
    {
        return OS::PlatformTimer::ticksToSeconds( m_curTicks );
    }

    //----------------------------------------------------------------------
    CallbackID MasterClock::setInterval( const std::function<void()>& func, Milliseconds ms )
    {
        return _AttachCallback( func, ms, true );
    }

    //----------------------------------------------------------------------
    CallbackID MasterClock::setTimeout( const std::function<void()>& func, Milliseconds ms )
    {
        return _AttachCallback( func, ms, false );
    }

    //----------------------------------------------------------------------
    void MasterClock::clearCallback( CallbackID id )
    {
        if (id == INVALID_CALLBACK_ID)
            return;

        if (m_timers.count( id ) == 0)
        {
            LOG_WARN( "MasterClock::clearCallback(): Given CallbackID #" + TS( id ) + " does not exist." );
            return;
        }

        m_timers.erase( id );
    }

    //----------------------------------------------------------------------
    void MasterClock::_UpdateTimer()
    {
        static std::vector<CallbackID> m_idsToRemove;
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
    CallbackID MasterClock::_AttachCallback( const std::function<void()>& func, Milliseconds ms, bool loop )
    {
        U64 nextID = NextID();

        m_timers[nextID] = std::move( CallbackTimer( func, ms, loop ) );

        return nextID;
    }

} // end namespaces