#include "subsystem_manager.h"

/**********************************************************************
    class: SubSystemManager (subsystem_manager.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

#include "locator.h"
#include "Logging/console_logger.h"
#include "MemoryManagement/memory_manager.h"
#include "OS/FileSystem/virtual_file_system.h"
#include "Config/configuration_manager.h"
#include "Logging/shared_console_logger.hpp"
#include "ThreadManager/thread_manager.h"
#include "Profiling/profiler.h"
#include "Input/input_manager.h"

#define ENABLE_THREADING 0
#define ENABLE_CONFIG    0

namespace Core
{

    #define COLOR Color(0, 255, 255)

    //----------------------------------------------------------------------
    static bool INITIALIZED = false;
    SubSystemManager::SubSystemManager()
    {
        ASSERT( INITIALIZED == false && "Only one instance of this class is allowed!" );
        INITIALIZED = true;
    }


    //----------------------------------------------------------------------
    void SubSystemManager::init()
    {
        // The logger uses the virtual file-paths, so they have to be initialized first
        _InitVirtualFilePaths();

        //----------------------------------------------------------------------
        m_logger = initializeSubSystem( new Logging::SharedConsoleLogger() );

        LOG( "<<< Initialize Sub-Systems >>>", COLOR);
        LOG( " > Logger initialized!", COLOR);

        //----------------------------------------------------------------------
        m_memoryManager = initializeSubSystem( new MemoryManagement::MemoryManager() );
        LOG( " > MemoryManager initialized!", COLOR);

        //----------------------------------------------------------------------
#if ENABLE_CONFIG
        m_configManager = initializeSubSystem( new Config::ConfigurationManager() );
        LOG( " > ConfigurationManager initialized!", COLOR );
#endif
        //----------------------------------------------------------------------
#if ENABLE_THREADING
        m_threadManager = initializeSubSystem( new Threading::ThreadManager() );
        LOG( " > ThreadManager initialized!", COLOR );
#endif
        //----------------------------------------------------------------------
        m_profiler = initializeSubSystem( new Profiling::Profiler() );
        LOG( " > Profiler initialized!", COLOR );

        //----------------------------------------------------------------------
        m_inputManager = initializeSubSystem( new Input::InputManager() );
        LOG(" > InputManager initialized!", COLOR);
    }

    //----------------------------------------------------------------------
    void SubSystemManager::update( Time::Seconds delta )
    {
        m_logger->update( delta );
        m_memoryManager->update( delta );
#if ENABLE_CONFIG
        m_configManager->update( delta );
#endif
#if ENABLE_THREADING
        m_threadManager->update( delta );
#endif
        m_profiler->update( delta );
        m_inputManager->update( delta );
    }


    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        // Shutdown every Sub-System here in reversed order to above
        LOG( "<<< Shutting down Sub-Systems >>>", COLOR );

        //----------------------------------------------------------------------
        LOG(" > Shutdown InputManager...", COLOR);
        shutdownSubSystem( m_inputManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Profiler...", COLOR );
        shutdownSubSystem( m_profiler );

        //----------------------------------------------------------------------
#if ENABLE_THREADING
        LOG( " > Shutdown ThreadManager...", COLOR );
        shutdownSubSystem( m_threadManager );
#endif
        //----------------------------------------------------------------------
#if ENABLE_CONFIG
        LOG( " > Shutdown ConfigurationManager...", COLOR );
        shutdownSubSystem( m_configManager );
#endif
        //----------------------------------------------------------------------
        LOG( " > Shutdown MemoryManager...", COLOR);
        shutdownSubSystem( m_memoryManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Logger...", COLOR);
        shutdownSubSystem( m_logger );

        //----------------------------------------------------------------------
        _ClearVirtualFilePaths();
    }

    //----------------------------------------------------------------------
    void SubSystemManager::_InitVirtualFilePaths()
    {
        OS::VirtualFileSystem::mount( "config",   "res" );
        OS::VirtualFileSystem::mount( "logs",     "res/logs" );
        OS::VirtualFileSystem::mount( "textures", "res/textures" );
        OS::VirtualFileSystem::mount( "shaders",  "res/shaders" );
        OS::VirtualFileSystem::mount( "models",   "res/models" );
        OS::VirtualFileSystem::mount( "internal", "res/internal" );
        OS::VirtualFileSystem::mount( "cursors",  "res/internal/cursors" );
    }

    void SubSystemManager::_ClearVirtualFilePaths()
    {
        OS::VirtualFileSystem::unmountAll();
    }

}