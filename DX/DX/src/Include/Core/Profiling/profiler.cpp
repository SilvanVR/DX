#include "profiler.h"
/**********************************************************************
    class: Profiler (profiler.cpp)

    author: S. Hau
    date: October 28, 2017

    @Considerations:
      - Calculate the average of several samples across several frames
        and issue a warning if a new sample with a very different
        amount occurs.
**********************************************************************/

#include "Core/locator.h"
#include "Logging/logging.h"
#include "OS/PlatformTimer/platform_timer.h"

namespace Core { namespace Profiling {

    //----------------------------------------------------------------------
    #define LOGCOLOR Color::GREEN

    //----------------------------------------------------------------------
    void Profiler::init()
    {
        Locator::getCoreEngine().subscribe( this );
    }

    //----------------------------------------------------------------------
    void Profiler::OnUpdate(  Time::Seconds delta )
    {
        static U32 frameCounter = 0;
        static Time::Seconds secTimer = 0;

        frameCounter++;
        secTimer += delta;
        if (secTimer > 1.0f)
        {
            m_fps = frameCounter;

            secTimer -= 1.0f;
            frameCounter = 0;
        }

        m_updateDelta = delta;
    }

    //----------------------------------------------------------------------
    void Profiler::shutdown()
    {
        if ( m_entries.size() > 0 )
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
        if (m_entries.size() != 0)
        {
            LOG( " >>>> Profiling results: ", LOGCOLOR );
            for (auto& pair : m_entries)
            {
                StringID    name  = pair.first;
                U64         ticks = pair.second;

                F64 ms = OS::PlatformTimer::ticksToMilliSeconds( ticks );

                // Example: [Name]: 2ms
                LOG( "[" + name.toString() + "]: " + TS( ms ) + "ms", LOGCOLOR );
            }
        }
        else
        {
            LOG( "No Profiling results.", LOGCOLOR );
        }
    }

    //----------------------------------------------------------------------
    void Profiler::logGPU()
    {
        auto frameInfo = Locator::getRenderer().getLastFrameInfo();
        String str = "<<< Last Frame Info >>>\n"
            "Drawcalls: " + TS( frameInfo.drawCalls ) + "\n"
            "Vertices: " + TS( frameInfo.numVertices ) + "\n"
            "Triangles: " + TS( frameInfo.numTriangles );
        LOG( str, LOGCOLOR );
    }


} } // end namespaces