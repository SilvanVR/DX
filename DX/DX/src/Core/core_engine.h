#pragma once
/**********************************************************************
    class: CoreEngine (core_engine.h)
    
    author: S. Hau
    date: October 27, 2017

    Terminology:
     - Tick()   : The game tick rate. Usually 60x per second.
     - Update() : Runs every frame as fast as possible.

    The heart of the engine. Manages the core game loop.
    @Considerations:
     - Terminate engine from everywhere
**********************************************************************/

#include "subsystem_manager.h"
#include "Time/master_clock.h"
#include "OS/Window/window.h"

namespace Core {

    //----------------------------------------------------------------------
    namespace OS { class Window; }
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
        OS::Window&         getWindow()         { return m_window; }
        Time::MasterClock&  getMasterClock()    { return m_engineClock; }

        //----------------------------------------------------------------------
        // Initiate the startup sequence for the engine.
        //----------------------------------------------------------------------
        void start(const char* title, U32 width, U32 height);

        //----------------------------------------------------------------------
        // Stops the core game loop.
        //----------------------------------------------------------------------
        void terminate() { m_isRunning = false; m_window.destroy(); };

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
        //----------------------------------------------------------------------
        void subscribe(ISubSystem* subSystem) { m_subscribers.push_back( subSystem ); }

    private:
        Time::MasterClock           m_engineClock;
        SubSystemManager            m_subSystemManager;
        OS::Window                  m_window;
        std::vector<ISubSystem*>    m_subscribers;
        bool                        m_isRunning = true;

        //----------------------------------------------------------------------
        void _RunCoreGameLoop();
        void _Shutdown();

        void _NotifyOnTick(Time::Seconds delta);
        void _NotifyOnUpdate(Time::Seconds delta);

        //----------------------------------------------------------------------
        CoreEngine(const CoreEngine& other)                 = delete;
        CoreEngine& operator = (const CoreEngine& other)    = delete;
        CoreEngine(CoreEngine&& other)                      = delete;
        CoreEngine& operator = (CoreEngine&& other)         = delete;
    };



} // end namespaces
