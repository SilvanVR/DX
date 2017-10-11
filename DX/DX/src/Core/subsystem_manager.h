#pragma once

/**********************************************************************
    class: SubSystemManager (subsystem_manager.h)

    author: S. Hau
    date: October 11, 2017

    Manages every Sub-System in the engine.
    Creates, initializes, deinitializes & destroys them.
**********************************************************************/

#include "Logging/i_logger.hpp"

namespace Core {

    class SubSystemManager
    {
    public:
        SubSystemManager();

        //----------------------------------------------------------------------
        // Initializes / Deinitializes every Sub-System
        //----------------------------------------------------------------------
        void init();
        void shutdown();

    private:
        //----------------------------------------------------------------------
        // Every Sub-System enumerated here
        //----------------------------------------------------------------------
        Logging::ILogger* m_logger;

        template <class T>
        T* initializeSubSystem(T* system);

        template <class T>
        void shutdownSubSystem(T*& system);

        SubSystemManager(const SubSystemManager& other)                 = delete;
        SubSystemManager& operator = (const SubSystemManager& other)    = delete;
        SubSystemManager(SubSystemManager&& other)                      = delete;
        SubSystemManager& operator = (SubSystemManager&& other)         = delete;
    };

    template <class T>
    T* SubSystemManager::initializeSubSystem( T* system )
    {
        system->init();
        Locator::provide( system );

        return system;
    }

    template <class T>
    void SubSystemManager::shutdownSubSystem( T*& system )
    {
        system->shutdown();
        delete system;
        system = nullptr;
        Locator::provide( (T*) nullptr );
    }


} // end namespaces