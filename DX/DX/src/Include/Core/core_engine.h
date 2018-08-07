#pragma once
/**********************************************************************
    class: CoreEngine (core_engine.h)
    
    author: S. Hau
    date: October 27, 2017

    Terminology:
     - Tick()   : The game tick rate. Usually 60x per second.
     - Update() : Runs every frame as fast as possible if vsync is disabled.
                  Otherwise it is capped to the vsync frequency.

    The heart of the engine. Manages the core game loop.
**********************************************************************/

#include "subsystem_manager.h"
#include "Time/master_clock.h"
#include "OS/Window/window.h"

namespace Core {

    //----------------------------------------------------------------------
    class ISubSystem;

    //**********************************************************************
    class CoreEngine
    {
    protected:
        static const U8 GAME_TICK_RATE = 60;

    public:
        CoreEngine() = default;
        virtual ~CoreEngine() = default;

        //----------------------------------------------------------------------
        OS::Window&         getWindow()             { return m_window; }
        Time::MasterClock&  getMasterClock()        { return m_engineClock; }
        U64                 getFrameCount() const   { return m_frameCounter; }

        //----------------------------------------------------------------------
        // Initiate the startup sequence for the engine.
        //----------------------------------------------------------------------
        void start(const char* title, U32 width, U32 height);

        //----------------------------------------------------------------------
        // Stops the core game loop.
        //----------------------------------------------------------------------
        void terminate() { m_isRunning = false; }

        //----------------------------------------------------------------------
        // Stops the core game loop.
        //----------------------------------------------------------------------
        void restart() { m_isRunning = false; m_restart = true; }

        //----------------------------------------------------------------------
        virtual void init() = 0;
        virtual void tick(Time::Seconds delta) = 0;
        virtual void shutdown() = 0;

        //----------------------------------------------------------------------
        // Subscribe to the core engine for the OnTick() + OnUpdate() Method.
        // The order of subscriptions determines the order of the notified
        // functions. It's guaranteed that OnTick() runs before the Game ticks.
        // @Params:
        //  "subSystem": The subsystem which should be notified.
        //  "insertFront": Insert the subsystem at the front.
        //----------------------------------------------------------------------
        void subscribe(ISubSystem* subSystem, bool insertFront = false);

    private:
        Time::MasterClock           m_engineClock;
        SubSystemManager            m_subSystemManager;
        OS::Window                  m_window;
        std::vector<ISubSystem*>    m_subscribers;
        bool                        m_isRunning = true;
        bool                        m_restart = false;
        U64                         m_frameCounter = 0;

        //----------------------------------------------------------------------
        void _RunCoreGameLoop();
        void _Shutdown();

        void _NotifyOnTick(Time::Seconds delta);
        void _NotifyOnUpdate(Time::Seconds delta);

        void _OnWindowSizeChanged(U16 w, U16 h);

        void _Render(F32 lerp);

        NULL_COPY_AND_ASSIGN(CoreEngine)
    };



} // end namespaces
