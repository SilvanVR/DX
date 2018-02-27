#include "subsystem_manager.h"

/**********************************************************************
    class: SubSystemManager (subsystem_manager.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

#include "locator.h"
#include "logging.h"
#include "Logging/console_logger.h"
#include "MemoryManager/memory_manager.h"
#include "OS/FileSystem/virtual_file_system.h"
#include "Config/configuration_manager.h"
#include "Logging/shared_console_logger.hpp"
#include "ThreadManager/thread_manager.h"
#include "Profiling/profiler.h"
#include "Input/input_manager.h"
#include "InGameConsole/in_game_console.h"
#include "Graphics/D3D11/D3D11Renderer.h"
#include "SceneManager/scene_manager.h"
#include "Resources/resource_manager.h"

//----------------------------------------------------------------------
#define ENABLE_THREADING 1
#define ENABLE_CONFIG    1

namespace Core
{
    //----------------------------------------------------------------------
    #define LOGCOLOR  Color(0, 255, 255)

    //----------------------------------------------------------------------
    SubSystemManager::SubSystemManager()
    {
        static bool INITIALIZED = false;
        ASSERT( INITIALIZED == false && "Only one instance of this class is allowed!" );
        INITIALIZED = true;
    }

    //----------------------------------------------------------------------
    void SubSystemManager::init()
    {
        // The logger uses the virtual file-paths, so they have to be initialized first
        _InitVirtualFilePaths();

        //----------------------------------------------------------------------
        gLogger.init();

        LOG( "<<< Initialize Sub-Systems >>>", LOGCOLOR );
        LOG( " > Logger initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_memoryManager = initializeSubSystem( new MemoryManagement::MemoryManager() );
        LOG( " > MemoryManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
#if ENABLE_CONFIG
        m_configManager = initializeSubSystem( new Config::ConfigurationManager() );
        LOG( " > ConfigurationManager initialized!", LOGCOLOR  );
#endif
        //----------------------------------------------------------------------
#if ENABLE_THREADING
        m_threadManager = initializeSubSystem( new Threading::ThreadManager() );
        LOG( " > ThreadManager initialized!", LOGCOLOR  );
#endif
        //----------------------------------------------------------------------
        m_profiler = initializeSubSystem( new Profiling::Profiler() );
        LOG( " > Profiler initialized!", LOGCOLOR  );

        //----------------------------------------------------------------------
        m_inputManager = initializeSubSystem( new Input::InputManager() );
        LOG( " > InputManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_inGameConsole = initializeSubSystem( new InGameConsole() );
        LOG( " > In-Game Console initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        ASSERT( &Locator::getWindow() != nullptr );
        m_renderer = initializeSubSystem( new Graphics::D3D11Renderer( &Locator::getWindow() ) );
        LOG( " > Renderer initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_sceneManager = initializeSubSystem( new SceneManager() );
        LOG( " > SceneManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_resourceManager = initializeSubSystem( new Resources::ResourceManager() );
        LOG( " > ResourceManager initialized!", LOGCOLOR );
    }

    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        // Shutdown every Sub-System here in reversed order to above
        LOG( "<<< Shutting down Sub-Systems >>>", LOGCOLOR  );

        //----------------------------------------------------------------------
        LOG( " > Shutdown ResourceManager...", LOGCOLOR );
        shutdownSubSystem( m_resourceManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown SceneManager...", LOGCOLOR );
        shutdownSubSystem( m_sceneManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Renderer...", LOGCOLOR );
        shutdownSubSystem( m_renderer );

        //----------------------------------------------------------------------
        LOG( " > Shutdown In-Game Console...", LOGCOLOR  );
        shutdownSubSystem( m_inGameConsole );

        //----------------------------------------------------------------------
        LOG(" > Shutdown InputManager...", LOGCOLOR );
        shutdownSubSystem( m_inputManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Profiler...", LOGCOLOR  );
        shutdownSubSystem( m_profiler );

        //----------------------------------------------------------------------
#if ENABLE_THREADING
        LOG( " > Shutdown ThreadManager...", LOGCOLOR  );
        shutdownSubSystem( m_threadManager );
#endif
        //----------------------------------------------------------------------
#if ENABLE_CONFIG
        LOG( " > Shutdown ConfigurationManager...", LOGCOLOR  );
        shutdownSubSystem( m_configManager );
#endif
        //----------------------------------------------------------------------
        LOG( " > Shutdown MemoryManager...", LOGCOLOR );
        shutdownSubSystem( m_memoryManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Logger...", LOGCOLOR );
        gLogger.shutdown();

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

    //----------------------------------------------------------------------
    void SubSystemManager::_ClearVirtualFilePaths()
    {
        OS::VirtualFileSystem::unmountAll();
    }

}