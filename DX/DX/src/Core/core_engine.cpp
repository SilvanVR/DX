#include "core_engine.h"

/**********************************************************************
    class: CoreEngine (core_engine.cpp)

    author: S. Hau
    date: October 27, 2017

    @Considerations:
      - Tick physics subsystem in a different fixed rate
**********************************************************************/

#include "locator.h"
#include "Misc/clock.h"

namespace Core {

    //----------------------------------------------------------------------
    void CoreEngine::start()
    {
        // Initialize all subsystems
        m_subSystemManager.init();

        // Call virtual init function for game class
        init();

        // Start core gameloop
        _RunCoreGameLoop();
    }

    void PrintFPS(F64 delta)
    {
        static U64 frameCounter = 0;
        static F64 secTimer = 0;

        frameCounter++;
        secTimer += delta;
        if (secTimer > 1.0f)
        {
            LOG("FPS: " + TS(frameCounter));
            secTimer = 0;
            frameCounter = 0;
        }
        //LOG( "FPS: " + TS( Locator::getProfiler().getFPS() ) );
    }

    //----------------------------------------------------------------------
    void CoreEngine::_RunCoreGameLoop()
    {
        constexpr F64 TICK_RATE_IN_SECONDS  = (1.0f / GAME_TICK_RATE);
        const     U8  MAX_TICKS_PER_FRAME   = 5; // Prevents the "spiral of death"

        Clock gameClock;
        F64 gameTickAccumulator = 0;

        LOG( "Entering Core-Loop." );
        while (m_isRunning)
        {
            // Update window class

            F64 delta = gameClock.getDelta();
            if (delta > 0.5f) delta = 0.5f;
            PrintFPS(delta);

            {
                // Update subsystems every frame
                m_subSystemManager.update();

                // Update game in fixed intervals
                U8 ticksPerFrame = 0;
                gameTickAccumulator += delta;
                while ( (gameTickAccumulator >= TICK_RATE_IN_SECONDS) && (ticksPerFrame++ != MAX_TICKS_PER_FRAME))
                {
                    tick( (F32)TICK_RATE_IN_SECONDS );
                    gameTickAccumulator -= TICK_RATE_IN_SECONDS;
                }

            }

            {
                // Render as fast as possible with interpolated state
                F64 lerp = (gameTickAccumulator / TICK_RATE_IN_SECONDS);
                ASSERT( lerp <= 1.0 );
                // m_renderer->render( lerp );
            }

        }

        _Shutdown();
    }


    //----------------------------------------------------------------------
    void CoreEngine::_Shutdown()
    {
        // Deinitialize game class
        shutdown();

        // Deinitialize every subsystem
        m_subSystemManager.shutdown();
    }


} // end namespaces