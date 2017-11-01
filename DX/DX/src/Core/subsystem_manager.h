#pragma once

/**********************************************************************
    class: SubSystemManager (subsystem_manager.h)

    author: S. Hau
    date: October 11, 2017

    See below for a class description.

**********************************************************************/

#include "Time/durations.h"

namespace Core {

    //----------------------------------------------------------------------
    // Forward Declarations
    //----------------------------------------------------------------------
    namespace Logging           { class ILogger; }
    namespace MemoryManagement  { class MemoryManager; }
    namespace Config            { class ConfigurationManager; }
    namespace Threading         { class ThreadManager; }
    namespace Profiling         { class Profiler; }


    //*********************************************************************
    // Manages every Sub-System in the engine.
    // Creates, initializes, deinitializes & destroys them.
    //*********************************************************************
    class SubSystemManager
    {
    public:
        SubSystemManager();

        //----------------------------------------------------------------------
        void init();
        void update(Time::Seconds delta);
        void shutdown();


    private:
        //----------------------------------------------------------------------
        // Every Sub-System is enumerated here
        //----------------------------------------------------------------------
        Logging::ILogger*                   m_logger        = nullptr;
        MemoryManagement::MemoryManager*    m_memoryManager = nullptr;
        Config::ConfigurationManager*       m_configManager = nullptr;
        Threading::ThreadManager*           m_threadManager = nullptr;
        Profiling::Profiler*                m_profiler      = nullptr;


        //----------------------------------------------------------------------
        void _InitVirtualFilePaths();
        void _ClearVirtualFilePaths();

        //----------------------------------------------------------------------
        template <class T>
        T* initializeSubSystem(T* system);

        template <class T>
        void shutdownSubSystem(T*& system);

        //----------------------------------------------------------------------
        SubSystemManager(const SubSystemManager& other)                 = delete;
        SubSystemManager& operator = (const SubSystemManager& other)    = delete;
        SubSystemManager(SubSystemManager&& other)                      = delete;
        SubSystemManager& operator = (SubSystemManager&& other)         = delete;
    };

    //**********************************************************************
    // IMPLEMENTATION
    //**********************************************************************

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
        Locator::provide( (T*) nullptr );

        delete system;
        system = nullptr;
    }


} // end namespaces