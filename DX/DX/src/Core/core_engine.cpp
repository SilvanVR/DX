#include "core_engine.h"

/**********************************************************************
    class: CoreEngine (core_engine.cpp)

    author: S. Hau
    date: October 27, 2017

    @Considerations:
      - Tick physics subsystem in a different rate
**********************************************************************/

#include "locator.h"

namespace Core {

    //----------------------------------------------------------------------
    void CoreEngine::start()
    {
        // Provide game clock to the locator class
        Locator::provide( &m_engineClock );

        // Initialize all subsystems
        m_subSystemManager.init();

        // Call virtual init function for game class
        init();

        // Start core gameloop
        _RunCoreGameLoop();
    }

    //----------------------------------------------------------------------
    void CoreEngine::_RunCoreGameLoop()
    {
        const Time::Seconds TICK_RATE_IN_SECONDS  = (1.0f / GAME_TICK_RATE);
        const           U8  MAX_TICKS_PER_FRAME   = 5; // Prevents the "spiral of death"

        Time::Seconds gameTickAccumulator = 0;

        while (m_isRunning)
        {
            Time::Seconds delta = m_engineClock._Update();
            //if (delta > 0.5f) delta = 0.5f;

            {
                // Update subsystems every frame
                m_subSystemManager.update( delta );

                // Update game in fixed intervals
                U8 ticksPerFrame = 0;
                gameTickAccumulator += delta;
                while ( (gameTickAccumulator >= TICK_RATE_IN_SECONDS) && (ticksPerFrame++ != MAX_TICKS_PER_FRAME))
                {
                    tick( TICK_RATE_IN_SECONDS );
                    gameTickAccumulator -= TICK_RATE_IN_SECONDS;
                }
            }

            {
                // Render as fast as possible with interpolated state
                F64 lerp = (gameTickAccumulator / TICK_RATE_IN_SECONDS);
                //ASSERT( lerp <= 1.0 );
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

        LOG(" ~ Goodbye! ~ ", Color::GREEN);

        // Deinitialize every subsystem
        m_subSystemManager.shutdown();
    }


} // end namespaces