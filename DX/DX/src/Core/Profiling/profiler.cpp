#include "profiler.h"
/**********************************************************************
    class: Profiler (profiler.cpp)

    author: S. Hau
    date: October 28, 2017
**********************************************************************/

#include "locator.h"

namespace Core { namespace Profiling {

    //----------------------------------------------------------------------
    void Profiler::init()
    {

        //CallbackID id = Locator::getMasterClock().addInterval([&] {
        //    static U32 frameCounter = 0;
        //    m_fps = frameCounter;
        //}, 1000 );

    }

    //----------------------------------------------------------------------
    void Profiler::update( F32 delta )
    {
        static U32 frameCounter = 0;
        static F32 secTimer = 0;

        frameCounter++;
        secTimer += delta;
        if (secTimer > 1.0)
        {
            m_fps = frameCounter;

            secTimer -= 1.0;
            frameCounter = 0;
        }
    }

    //----------------------------------------------------------------------
    void Profiler::shutdown()
    {

    }


} } // end namespaces