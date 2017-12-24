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
    void CoreEngine::start( const char* title, U32 width, U32 height )
    {
        // Set core engine instance in the locator class
        Locator::setCoreEngine( this );

        // Create Window
        m_window.create( title, width, height );

        // Provide engine clock and window to the locator class
        Locator::provide( &m_engineClock );
        Locator::provide( &m_window );

        // Initialize all subsystems
        m_subSystemManager.init();

        // Initialize graphics recorder AFTER all subsystems
        m_graphicsCommandRecorder.init();

        // Call virtual init function for game class
        init();

        // Start core gameloop
        _RunCoreGameLoop();
    }

    //----------------------------------------------------------------------
    void CoreEngine::_RunCoreGameLoop()
    {
        const Time::Seconds TICK_RATE_IN_SECONDS  = (1.0f / GAME_TICK_RATE);
        const           U8  MAX_TICKS_PER_FRAME   = 5; // Prevents the "spiral of death" if the cpu is too slow

        Time::Seconds gameTickAccumulator = 0;

        while ( m_isRunning && not m_window.shouldBeClosed() )
        {
            Time::Seconds delta = m_engineClock._Update();
            //if (delta > 0.5f) delta = 0.5f;

            {
                _NotifyOnUpdate( delta );

                // Tick game in fixed intervals
                U8 ticksPerFrame = 0;
                gameTickAccumulator += delta;
                while ( (gameTickAccumulator >= TICK_RATE_IN_SECONDS) && (ticksPerFrame++ != MAX_TICKS_PER_FRAME))
                {
                    _NotifyOnTick( TICK_RATE_IN_SECONDS );

                    tick( TICK_RATE_IN_SECONDS );
                    gameTickAccumulator -= TICK_RATE_IN_SECONDS;
                }
            }

            {
                // Render as fast as possible with interpolated state
                F64 lerp = (F64)(gameTickAccumulator / TICK_RATE_IN_SECONDS);
                //ASSERT( lerp <= 1.0 );


                m_graphicsCommandRecorder.render( &SCENE, (F32)lerp );
            }

            m_window.processOSMessages();
        }

        _Shutdown();
    }


    //----------------------------------------------------------------------
    void CoreEngine::_Shutdown()
    {
        // Deinitialize game class
        shutdown();

        // Deinitialize graphics recorder
        m_graphicsCommandRecorder.shutdown();

        LOG(" ~ Goodbye! ~ ", Color::GREEN);

        // Deinitialize every subsystem
        m_subSystemManager.shutdown();
    }

    //----------------------------------------------------------------------
    void CoreEngine::_NotifyOnTick(Time::Seconds delta)
    {
        for (auto& subscriber : m_subscribers)
        {
            subscriber->OnTick( delta );
        }
    }

    //----------------------------------------------------------------------
    void CoreEngine::_NotifyOnUpdate(Time::Seconds delta)
    {
        for (auto& subscriber : m_subscribers)
        {
            subscriber->OnUpdate( delta );
        }
    }


} // end namespaces