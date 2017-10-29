#include "profiler.h"
/**********************************************************************
    class: Profiler (profiler.cpp)

    author: S. Hau
    date: October 28, 2017

    @Considerations:
      - Calculate the average of each sample
        and issue a warning if a new sample with a very different
        amount occurs.
**********************************************************************/

#include "locator.h"
#include "Core/OS/PlatformTimer/platform_timer.h"

namespace Core { namespace Profiling {

    //----------------------------------------------------------------------
    void Profiler::init()
    {
    }

    //----------------------------------------------------------------------
    void Profiler::update( F32 delta )
    {
        static U32 frameCounter = 0;
        static F32 secTimer = 0;

        frameCounter++;
        secTimer += delta;
        if (secTimer > 1.0f)
        {
            m_fps = frameCounter;

            secTimer -= 1.0f;
            frameCounter = 0;
        }
    }

    //----------------------------------------------------------------------
    void Profiler::shutdown()
    {
        log();
    }

    //----------------------------------------------------------------------
    void Profiler::profileCodeSectionBegin( const char* name )
    {
        StringID id = StringID( name );
        U64 beginTicks = OS::PlatformTimer::getTicks();
        m_entries[id] = beginTicks;
    }

    //----------------------------------------------------------------------
    void Profiler::profileCodeSectionEnd( const char* name )
    {
        StringID id = StringID( name );
        ASSERT( m_entries.count(id) != 0 );

        U64 endTicks = OS::PlatformTimer::getTicks();
        m_entries[id] = (endTicks - m_entries[id]);
    }

    //----------------------------------------------------------------------
    F64 Profiler::getCodeSectionTime( const char* name )
    {
        StringID id = StringID(name);
        ASSERT( m_entries.count(id) != 0 );

        return OS::PlatformTimer::ticksToMilliSeconds( m_entries[id] );
    }

    //----------------------------------------------------------------------
    void Profiler::log()
    {
        Color color = Color::GREEN;
        LOG( " >>>> Profiling results: ", color );
        for (auto& pair : m_entries)
        {
            StringID    name  = pair.first;
            U64         ticks = pair.second;

            F64 ms = OS::PlatformTimer::ticksToMilliSeconds( ticks );

            // [Name]: 2ms
            LOG( "[" + name.toString() + "]: " + TS( ms ) + "ms", color );
        }
    }


} } // end namespaces