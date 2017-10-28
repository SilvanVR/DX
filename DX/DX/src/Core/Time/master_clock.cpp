#include "master_clock.h"
/**********************************************************************
    class: Clock (master_clock.cpp)
    
    author: S. Hau
    date: October 27, 2017
**********************************************************************/

#include "Core/OS/PlatformTimer/platform_timer.h"

namespace Core { namespace Time {

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
        return m_delta;
    }


    //----------------------------------------------------------------------
    F64 MasterClock::getTime()
    {
        return OS::PlatformTimer::ticksToSeconds( getCurTicks() );
    }


} } // end namespaces