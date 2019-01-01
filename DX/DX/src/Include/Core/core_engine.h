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
#include "Graphics/enums.hpp"

namespace Core {

    //----------------------------------------------------------------------
    class ISubSystem;

    //----------------------------------------------------------------------
    enum class EGameLoopTechnique
    {
        Fixed,     // Updates GAME_TICK_RATE per second, render as fast as possible
        Variable   // Updates and renders as fast as possible
    };

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
        Graphics::API       getAPI()        const   { return m_api; }

        //----------------------------------------------------------------------
        // Initiate the startup sequence for the engine.
        //----------------------------------------------------------------------
        void start(const char* title, U32 width, U32 height, Graphics::API api = Graphics::API::D3D11);

        //----------------------------------------------------------------------
        // Stops the core game loop.
        //----------------------------------------------------------------------
        void terminate() { m_isRunning = false; }

        //----------------------------------------------------------------------
        // Restarts the whole engine.
        //----------------------------------------------------------------------
        void restart() { m_isRunning = false; m_restart = true; }

        //----------------------------------------------------------------------
        // Set's the graphics api and restarts the whole engine.
        //----------------------------------------------------------------------
        void setAPI(Graphics::API api) { m_api = api; restart(); }

        //----------------------------------------------------------------------
        void setGameLoopTechnique(EGameLoopTechnique technique) { m_gameLoopTechnique = technique; }

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
        U64                         m_frameCounter = 0;
        Graphics::API               m_api;
        bool                        m_isRunning = true;
        bool                        m_restart = true;
        EGameLoopTechnique          m_gameLoopTechnique = EGameLoopTechnique::Fixed;

        //----------------------------------------------------------------------
        void _Init(const char* title, U32 width, U32 height, Graphics::API api);
        void _RunCoreGameLoop();
        void _Shutdown();

        void _NotifyOnTick(Time::Seconds delta);
        void _NotifyOnUpdate(Time::Seconds delta);

        void _Render();

        NULL_COPY_AND_ASSIGN(CoreEngine)
    };



} // end namespaces
