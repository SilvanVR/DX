#include "subsystem_manager.h"

/**********************************************************************
    class: SubSystemManager (subsystem_manager.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

#include "locator.h"
#include "Logging/console_logger.h"
#include "MemoryManagement/memory_manager.h"
#include "VirtualFileSystem/virtual_file_system.h"

namespace Core
{

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
        // Initialize Logger first before anything
        m_logger = initializeSubSystem( new Logging::ConsoleLogger() );

        LOG( "Initialize Sub-Systems..." );

        // Initialize every Sub-System here
        _InitVirtualFilePaths();

        LOG("Initialize Memory-Manager...");
        m_memoryManager = initializeSubSystem( new MemoryManagement::MemoryManager() );

    }




    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        LOG( "Shutting down Sub-Systems..." );

        // Shutdown every Sub-System here in reversed order to above

        LOG( "Shutdown MemoryManager..." );
        shutdownSubSystem( m_memoryManager );

        // Delete Logger at last
        LOG( "Shutdown Logger..." );
        shutdownSubSystem( m_logger );
    }

    //----------------------------------------------------------------------
    void SubSystemManager::_InitVirtualFilePaths()
    {
        VirtualFileSystem::mount("logs",     "res/logs");
        VirtualFileSystem::mount("textures", "res/textures");
        VirtualFileSystem::mount("shaders",  "res/shaders");
        VirtualFileSystem::mount("models",   "res/models");
    }

    void SubSystemManager::_ShutdownVirtualFilePaths()
    {
        VirtualFileSystem::unmountAll();
    }

}