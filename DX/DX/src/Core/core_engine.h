#pragma once
/**********************************************************************
    class: CoreEngine (core_engine.h)
    
    author: S. Hau
    date: October 27, 2017

    The heart of the engine. Manages the core game loop.
    @Considerations:
     - Terminate engine from everywhere
**********************************************************************/

#include "subsystem_manager.h"
#include "Time/master_clock.h"

namespace Core {

    //**********************************************************************
    class CoreEngine
    {
    protected:
        static const U8 GAME_TICK_RATE = 60;

    public:
        CoreEngine() = default;
        virtual ~CoreEngine() = default;

        //----------------------------------------------------------------------
        // Initiate the startup sequence for the engine.
        //----------------------------------------------------------------------
        void start();

        //----------------------------------------------------------------------
        // Stops the core game loop.
        //----------------------------------------------------------------------
        void terminate() { m_isRunning = false; };

        //----------------------------------------------------------------------
        virtual void init() = 0;
        virtual void tick(Time::Seconds delta) = 0;
        virtual void shutdown() = 0;

    private:
        Time::MasterClock   m_engineClock;
        SubSystemManager    m_subSystemManager;
        bool                m_isRunning = true;

        //----------------------------------------------------------------------
        void _RunCoreGameLoop();
        void _Shutdown();

        //----------------------------------------------------------------------
        CoreEngine(const CoreEngine& other)                 = delete;
        CoreEngine& operator = (const CoreEngine& other)    = delete;
        CoreEngine(CoreEngine&& other)                      = delete;
        CoreEngine& operator = (CoreEngine&& other)         = delete;
    };



} // end namespaces
