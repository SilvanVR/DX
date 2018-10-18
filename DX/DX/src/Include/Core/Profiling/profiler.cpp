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
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/Components/Rendering/camera.h"

namespace Core { namespace Profiling {

    //----------------------------------------------------------------------
    #define LOGCOLOR Color::GREEN

    //----------------------------------------------------------------------
    void Profiler::init()
    {
        Locator::getCoreEngine().subscribe( this );
    }

    //----------------------------------------------------------------------
    void Profiler::OnTick( Time::Seconds delta )
    {
        m_tickDelta = delta;
    }

    //----------------------------------------------------------------------
    void Profiler::OnUpdate( Time::Seconds delta )
    {
        static U32 frameCounter = 0;
        static Time::Seconds secTimer = 0;

        frameCounter++;
        secTimer += delta;
        if (secTimer > 1_s)
        {
            m_fps = frameCounter;

            secTimer -= 1.0f;
            frameCounter = 0;
        }

        m_updateDelta = delta;

        if (m_profileCallback)
        {
            m_profileFrameTimes.push_back( delta );
            m_profileTime += delta;
            if (m_profileTime > m_profileDuration)
                _EndProfile();
        }
    }

    //----------------------------------------------------------------------
    void Profiler::shutdown()
    {
        if ( m_entries.size() > 0 )
            log();
    }

    //----------------------------------------------------------------------
    // PUBLIC
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void Profiler::profileCodeSectionBegin( const char* name )
    {
        StringID id = SID( name );
        U64 beginTicks = OS::PlatformTimer::getTicks();
        m_entries[id] = beginTicks;
    }

    //----------------------------------------------------------------------
    void Profiler::profileCodeSectionEnd( const char* name )
    {
        StringID id = SID( name );
        ASSERT( m_entries.count(id) != 0 );

        U64 endTicks = OS::PlatformTimer::getTicks();
        m_entries[id] = (endTicks - m_entries[id]);
    }

    //----------------------------------------------------------------------
    F64 Profiler::getCodeSectionTime( const char* name )
    {
        StringID id = SID( name );
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
            LOG( "No Standard Profiling results.", LOGCOLOR );
        }
    }

    //----------------------------------------------------------------------
    void Profiler::logGPU()
    {
        auto gpuDesc = Locator::getRenderer().getGPUDescription();
        String str = "--- " + gpuDesc.name + " (" + TS( gpuDesc.maxDedicatedMemoryMB ) + "MB) ---\n";

        // Print stats from all cameras
        auto cameras = SCENE.getComponentManager().getCameras();
        I32 cameraIndex = 0;
        for ( auto& cam : cameras )
        {
            auto& frameInfo = cam->getFrameInfo();
            str += "<<< Camera #" + TS( cameraIndex++ ) + " >>>\n";
            str += "Drawcalls: " + TS( frameInfo.drawCalls ) + "\n";
            str += "Vertices: " + TS( frameInfo.numVertices ) + "\n";
            str += "Triangles: " + TS( frameInfo.numTriangles ) + "\n";
            str += "Lights: " + TS( frameInfo.numLights ) + "\n";
        }
        LOG( str, LOGCOLOR );
    }

    //----------------------------------------------------------------------
    void Profiler::beginProfiling( Time::Seconds duration, std::function<void(ProfileResult)> callback )
    {
        if (m_profileCallback)
        {
            LOG_WARN( "Profiler::beginProfiling(): Already profiling. Wait until current profiling ends." );
            return;
        }
        m_profileTime = 0_s;
        m_profileDuration = duration;
        m_profileCallback = callback;
        m_profileFrameTimes.clear();
        LOG( "[Profiler] Begin profiling for " + TS(duration.value) + " seconds..." );
    }

    //----------------------------------------------------------------------
    // PRIVATE
    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    void Profiler::_EndProfile()
    {
        LOG( "[Profiler] End profiling..." );

        ProfileResult result{};
        result.minFrameTime = 999999_ms;
        result.numFrames = static_cast<U64>( m_profileFrameTimes.size() );

        Time::Milliseconds sum;
        for (const auto t : m_profileFrameTimes)
        {
            sum += t;
            result.minFrameTime = std::min( result.minFrameTime, t );
            result.maxFrameTime = std::max( result.maxFrameTime, t );
        }
        result.avgFrameTime = sum / result.numFrames;

        m_profileCallback( result );
        m_profileCallback = nullptr;
    }


} } // end namespaces