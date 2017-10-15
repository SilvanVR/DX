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
        // Initialize Logger first
        m_logger = initializeSubSystem( new Logging::ConsoleLogger() );

        // Initialize memory manager
        m_memoryManager = initializeSubSystem( new MemoryManagement::MemoryManager() );


        // Initialize every Sub-System here
        LOG( "Initialize Sub-Systems..." );
        //_InitVirtualFilePaths();

    }




    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        LOG( "Shutting down Sub-Systems..." );

        // Shutdown every Sub-System here in reversed order to above
        //_ShutdownVirtualFilePaths();



        // Shutdown memory manager before logger, after anything else
        LOG( "Shutdown MemoryManager..." );
        m_logger->_DumpToDisk(); // This is necessary
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