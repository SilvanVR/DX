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
        m_logger = initializeSubSystem( new Logging::ConsoleLogger() );

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
        m_configManager = initializeSubSystem( new Config::ConfigurationManager() );
        LOG( " > ConfigurationManager initialized!", COLOR );

        //----------------------------------------------------------------------


    }


    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        // Shutdown every Sub-System here in reversed order to above
        LOG( "<<< Shutting down Sub-Systems >>>", COLOR);


        //----------------------------------------------------------------------
        LOG( " > Shutdown ConfigurationManager...", COLOR );
        shutdownSubSystem( m_configManager );

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
        _ShutdownVirtualFilePaths();
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

    void SubSystemManager::_ShutdownVirtualFilePaths()
    {
        OS::VirtualFileSystem::unmountAll();
    }

}