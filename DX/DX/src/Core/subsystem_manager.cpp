#include "subsystem_manager.h"

/**********************************************************************
    class: SubSystemManager (subsystem_manager.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

#include "locator.h"
#include "Logging/logging.h"
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
#include "Assets/asset_manager.h"
#include "DebugManager/debug_manager.h"
#include "Audio/audio_manager.h"

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
        gLogger = new Logging::SharedConsoleLogger();

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
        m_resourceManager = initializeSubSystem( new Resources::ResourceManager() );
        LOG( " > ResourceManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_audioManager = initializeSubSystem( new Audio::AudioManager() );
        LOG( " > AudioManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_sceneManager = initializeSubSystem( new SceneManager() );
        LOG( " > SceneManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_assetManager = initializeSubSystem( new Assets::AssetManager() );
        LOG(" > AssetManager initialized!", LOGCOLOR );

        //----------------------------------------------------------------------
        m_debugManager = initializeSubSystem( new Debug::DebugManager() );
        LOG( " > DebugManager initialized!", LOGCOLOR );
    }

    //----------------------------------------------------------------------
    void SubSystemManager::shutdown()
    {
        // Shutdown every Sub-System here in reversed order to above (almost, some systems should be shutdown before others)
        LOG( "<<< Shutting down Sub-Systems >>>", LOGCOLOR  );

        //----------------------------------------------------------------------
#if ENABLE_THREADING
        LOG( " > Shutdown ThreadManager...", LOGCOLOR  );
        shutdownSubSystem( m_threadManager );
#endif
        //----------------------------------------------------------------------
        LOG( " > Shutdown DebugManager...", LOGCOLOR );
        shutdownSubSystem( m_debugManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown AssetManager...", LOGCOLOR );
        shutdownSubSystem( m_assetManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown SceneManager...", LOGCOLOR );
        shutdownSubSystem( m_sceneManager );

        //----------------------------------------------------------------------
        LOG(" > Shutdown AudioManager...", LOGCOLOR);
        shutdownSubSystem( m_audioManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Renderer...", LOGCOLOR );
        shutdownSubSystem( m_renderer ); // must be deleted before resource manager, so global material handles get freed

        //----------------------------------------------------------------------
        LOG( " > Shutdown ResourceManager...", LOGCOLOR );
        shutdownSubSystem( m_resourceManager );

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
#if ENABLE_CONFIG
        LOG( " > Shutdown ConfigurationManager...", LOGCOLOR  );
        shutdownSubSystem( m_configManager );
#endif
        //----------------------------------------------------------------------
        LOG( " > Shutdown MemoryManager...", LOGCOLOR );
        shutdownSubSystem( m_memoryManager );

        //----------------------------------------------------------------------
        LOG( " > Shutdown Logger...", LOGCOLOR );
        SAFE_DELETE( gLogger );
        Logging::ResetToNullLogger();

        //----------------------------------------------------------------------
        _ClearVirtualFilePaths();
    }

    //----------------------------------------------------------------------
    void SubSystemManager::_InitVirtualFilePaths()
    {
        OS::VirtualFileSystem::mount( "config",   "../DX/res" );
        OS::VirtualFileSystem::mount( "logs",     "../DX/res/logs" );
        OS::VirtualFileSystem::mount( "textures", "../DX/res/textures" );
        OS::VirtualFileSystem::mount( "cubemaps", "../DX/res/textures/cubemaps" );
        OS::VirtualFileSystem::mount( "shaders",  "../DX/res/shaders" );
        OS::VirtualFileSystem::mount( "models",   "../DX/res/models" );
        OS::VirtualFileSystem::mount( "internal", "../DX/res/internal" );
        OS::VirtualFileSystem::mount( "cursors",  "../DX/res/internal/cursors" );
        OS::VirtualFileSystem::mount( "audio",    "../DX/res/audio" );
    }

    //----------------------------------------------------------------------
    void SubSystemManager::_ClearVirtualFilePaths()
    {
        OS::VirtualFileSystem::unmountAll();
    }

}