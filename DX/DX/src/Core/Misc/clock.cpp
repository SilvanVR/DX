#include "clock.h"
/**********************************************************************
    class: Clock (clock.cpp)
    
    author: S. Hau
    date: October 27, 2017
**********************************************************************/

#include "Core/OS/PlatformTimer/platform_timer.h"


namespace Core {

    //----------------------------------------------------------------------
    Clock::Clock(bool loop)
        : m_loop( loop ), m_startTicks( OS::PlatformTimer::getTicks() )
    {
    }

    //----------------------------------------------------------------------
    F64 Clock::getDelta()
    {
        static U64 lastTicks = 0;
        U64 curTicks = getCurTicks();
        U64 deltaTicks = curTicks - lastTicks;
        lastTicks = curTicks;

        return OS::PlatformTimer::ticksToSeconds( deltaTicks );
    }

    //----------------------------------------------------------------------
    U64 Clock::getCurTicks()
    {
        return (OS::PlatformTimer::getTicks() - m_startTicks);
    }

    //----------------------------------------------------------------------
    F64 Clock::getTime()
    {
        return OS::PlatformTimer::ticksToSeconds( getCurTicks() );
    }


} // end namespaces