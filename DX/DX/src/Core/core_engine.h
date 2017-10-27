#pragma once
/**********************************************************************
    class: CoreEngine (core_engine.h)
    
    author: S. Hau
    date: October 27, 2017

    The heart of the engine. Manages the core game loop.
**********************************************************************/

#include "subsystem_manager.h"

namespace Core {


    class CoreEngine
    {
        static const U8 CORE_UPDATE_RATE      = 60;
        static const U8 PHYSICS_UPDATE_RATE   = 100;

    public:
        CoreEngine() = default;
        virtual ~CoreEngine() = default;

        //----------------------------------------------------------------------
        // Initiate the startup sequence for the engine.
        //----------------------------------------------------------------------
        void start();

        //----------------------------------------------------------------------
        // Immediately stops the core game loop.
        //----------------------------------------------------------------------
        void terminate() { m_isRunning = false; };

        //----------------------------------------------------------------------
        virtual void init() = 0;
        virtual void tick(F32 delta) = 0;
        virtual void shutdown() = 0;

    private:
        SubSystemManager m_subSystemManager;
        bool             m_isRunning = true;

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
