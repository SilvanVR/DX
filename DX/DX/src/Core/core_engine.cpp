#include "core_engine.h"

/**********************************************************************
    class: CoreEngine (core_engine.cpp)

    author: S. Hau
    date: October 27, 2017
**********************************************************************/

#include "locator.h"

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

    //----------------------------------------------------------------------
    void CoreEngine::_RunCoreGameLoop()
    {
        constexpr F64 updateRateInSeconds           = (1.0f / CORE_UPDATE_RATE);
        constexpr F64 physicsUpdateRateInSeconds    = (1.0f / PHYSICS_UPDATE_RATE);

        LOG( "Entering Core-Loop." );
        while (m_isRunning)
        {
            // Update window class
            //m_masterClock.update();

            // Update subsystems every frame
            m_subSystemManager.update();

            // Update game in fixed intervals
            tick(0);


            // Render as fast as possible with interpolated state
            // F32 lerp = 0.0f;
            // m_renderer->render( lerp );
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