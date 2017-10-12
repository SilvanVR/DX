#include "platform_timer.h"

/**********************************************************************
    class: PlatformTimer (platform_timer.cpp)

    author: S. Hau
    date: October 12, 2017

**********************************************************************/

namespace Core { namespace OS {


    //----------------------------------------------------------------------
    PlatformTimer   PlatformTimer::m_instance;
    I64             PlatformTimer::m_tickFrequency = 0;
    F64             PlatformTimer::m_tickFrequencyInSeconds = 0;
    //----------------------------------------------------------------------


} }