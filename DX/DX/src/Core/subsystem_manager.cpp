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
        // String table needs to be created after the memory manager but before anything else
        _CreateStringTable();
        LOG( " > StringTable created!", COLOR );

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


    }

    //----------------------------------------------------------------------
    void SubSystemManager::update()
    {
        m_logger->update();
        m_memoryManager->update();
#if ENABLE_CONFIG
        m_configManager->update();
#endif
#if ENABLE_THREADING
        m_threadManager->update();
#endif
    }


    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        // Shutdown every Sub-System here in reversed order to above
        LOG( "<<< Shutting down Sub-Systems >>>", COLOR );

        //----------------------------------------------------------------------



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
        LOG( " > Destroy StringTable..." , COLOR);
        _DestroyStringTable();

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
        OS::VirtualFileSystem::mount("config",   "res");
        OS::VirtualFileSystem::mount("logs",     "res/logs");
        OS::VirtualFileSystem::mount("textures", "res/textures");
        OS::VirtualFileSystem::mount("shaders",  "res/shaders");
        OS::VirtualFileSystem::mount("models",   "res/models");
    }

    void SubSystemManager::_ClearVirtualFilePaths()
    {
        OS::VirtualFileSystem::unmountAll();
    }

}