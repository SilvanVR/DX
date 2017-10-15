#include "subsystem_manager.h"

/**********************************************************************
    class: SubSystemManager (subsystem_manager.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

#include "locator.h"
#include "Logging/console_logger.h"
#include "MemoryManagement/memory_manager.h"
#include "OS/VirtualFileSystem/virtual_file_system.h"

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
        _InitVirtualFilePaths();

        // Initialize Logger first
        m_logger = initializeSubSystem( new Logging::ConsoleLogger() );

        // Initialize memory manager after logger, before anything else
        m_memoryManager = initializeSubSystem( new MemoryManagement::MemoryManager() );

        // Initialize every Sub-System here
        LOG( "<<< Initialize Sub-Systems >>>" );
        LOG( "Logger initialized!" );
        LOG( "MemoryManager initialized!" );


    }


    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        // Shutdown every Sub-System here in reversed order to above
        LOG( "<<< Shutting down Sub-Systems >>>" );




        //----------------------------------------------------------------------
        LOG( "Shutdown MemoryManager..." );
        // This is necessary, because the logger has to dump all allocated stuff, 
        // otherwise the memory manager will report a memory leak
        m_logger->_DumpToDisk();
        shutdownSubSystem( m_memoryManager );

        // Shutdown logger now
        LOG( "Shutdown Logger..." );
        shutdownSubSystem( m_logger );

        _ShutdownVirtualFilePaths();
    }

    //----------------------------------------------------------------------
    void SubSystemManager::_InitVirtualFilePaths()
    {
        OS::VirtualFileSystem::mount("logs",     "res/logs");
        OS::VirtualFileSystem::mount("textures", "res/textures");
        OS::VirtualFileSystem::mount("shaders",  "res/shaders");
        OS::VirtualFileSystem::mount("models",   "res/models");
    }

    void SubSystemManager::_ShutdownVirtualFilePaths()
    {
        OS::VirtualFileSystem::unmountAll();
    }

}