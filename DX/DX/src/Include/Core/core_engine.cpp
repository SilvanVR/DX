#include "core_engine.h"
/**********************************************************************
    class: CoreEngine (core_engine.cpp)

    author: S. Hau
    date: October 27, 2017

    @Considerations:
      - Tick physics subsystem in a different rate
**********************************************************************/

#include "Core/locator.h"
#include "Logging/logging.h"
#include "GameplayLayer/i_scene.h"
#include "GameplayLayer/Components/Rendering/camera.h"

namespace Core {

    //----------------------------------------------------------------------
    void CoreEngine::start( const char* title, U32 width, U32 height )
    {
        // Check for Math-Library support
        if ( not DirectX::XMVerifyCPUSupport() )
            LOG_ERROR( "DirectX-Math not supported on this system, but is required!" );

        // Set core engine instance in the locator class
        Locator::setCoreEngine( this );

        // Create Window & Attach window resize event
        m_window.create( title, width, height );
        m_window.setCallbackSizeChanged( BIND_THIS_FUNC_2_ARGS( &CoreEngine::_OnWindowSizeChanged ) );

        // Provide engine clock and window to the locator class
        Locator::provide( &m_engineClock );
        Locator::provide( &m_window );

        // Initialize all subsystems
        m_subSystemManager.init();

        // Call virtual init function for game class
        init();

        // Start core gameloop
        _RunCoreGameLoop();
    }

    //**********************************************************************
    // PUBLIC
    //**********************************************************************

    //----------------------------------------------------------------------
    void CoreEngine::subscribe( ISubSystem* subSystem, bool insertFront )
    { 
        if (insertFront)
            m_subscribers.insert( m_subscribers.begin(), subSystem );
        else
            m_subscribers.push_back( subSystem ); 
    }

    //**********************************************************************
    // PRIVATE
    //**********************************************************************

    //----------------------------------------------------------------------
    void CoreEngine::_RunCoreGameLoop()
    {
        const Time::Seconds TICK_RATE_IN_SECONDS  = (1.0f / GAME_TICK_RATE);
        const           U8  MAX_TICKS_PER_FRAME   = 5; // Prevents the "spiral of death" if the cpu is too slow

        Time::Seconds gameTickAccumulator = 0;

        while ( m_isRunning && not m_window.shouldBeClosed() )
        {
            Time::Seconds delta = m_engineClock._Update();
            if (delta > 0.5f) delta = 0.5f;

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
                _Render( (F32)lerp );
            }

            m_window.processOSMessages();
        }

        _Shutdown();
    }

    //----------------------------------------------------------------------
    void CoreEngine::_Render( F32 lerp )
    {
        auto& graphicsEngine = Locator::getRenderer();

        // Update global buffer on gpu
        Graphics::CommandBuffer globalBufferCommands;
        globalBufferCommands.setGlobalFloat( SID("gTime"), (F32)TIME.getTime() );
        globalBufferCommands.setGlobalFloat( SID("gAmbient"), 0.2f );
        Locator::getRenderer().dispatch( globalBufferCommands );

        // Records rendering commands for the current scene and dispatches them to the renderer
        graphicsEngine.resetFrameInfo();

        // Fetch all renderer components
        auto& renderers = SCENE.getComponentManager().getRenderer();

        // Create rendering commands for each camera and submit them to the rendering engine
        for ( auto& cam : SCENE.getComponentManager().getCameras() )
        {
            auto& cmd = cam->recordGraphicsCommands( lerp, renderers );

            // Send command buffer to rendering engine for execution
            graphicsEngine.dispatch( cmd );
        }

        // Present backbuffer to screen
        graphicsEngine.present();
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

    //----------------------------------------------------------------------
    void CoreEngine::_OnWindowSizeChanged( U16 w, U16 h )
    {
        Locator::getRenderer().OnWindowSizeChanged( w, h );
    }

} // end namespaces