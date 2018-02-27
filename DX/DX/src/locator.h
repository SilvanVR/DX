#pragma once
/**********************************************************************
    class: Locator (locator.h)

    author: S. Hau
    date: October 11, 2017

    Implements the Service-Locator Pattern.
    Every Sub-System can be retrieved via a static method.

    Features:
        [+/-] Every Sub-System in one place
        [+] Easy extensible
        [+] Exact retrieved Sub-System is only known at runtime when
            implemented a virtual base class for it
          [++] Every SubSystem can be easily swapped out
          [++] Hierarchies of a specific Sub-System are possible, e.g.
               Logged Audio!
          [--] Interface must be known a priori
          [--] More code + files
            [---] Templates are harder to use
          [--] Less performance because of virtual functions
**********************************************************************/

#include "Core/Logging/null_logger.hpp"
#include "Core/MemoryManagement/memory_manager.h"
#include "Core/Config/configuration_manager.h"
#include "Core/ThreadManager/thread_manager.h"
#include "Core/Profiling/profiler.h"
#include "Core/Time/master_clock.h"
#include "Core/OS/Window/window.h"
#include "Core/Input/input_manager.h"
#include "Core/core_engine.h"
#include "Core/InGameConsole/i_in_game_console.hpp"
#include "Core/Graphics/i_renderer.h"
#include "Core/SceneManager/scene_manager.h"
#include "Core/Resources/resource_manager.h"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                Locator::getLogger().log( Core::Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define WARN(...)               Locator::getLogger().warn( Core::Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define ERROR(...)              Locator::getLogger().error( Core::Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )

#define LOG_TEST(...)           Locator::getLogger().log( Core::Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )
#define WARN_TEST(...)          Locator::getLogger().warn( Core::Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )

#define LOG_MEMORY(...)         Locator::getLogger().log( Core::Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define WARN_MEMORY(...)        Locator::getLogger().warn( Core::Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define ERROR_MEMORY(...)       Locator::getLogger().error( Core::Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )

#define LOG_RENDERING(...)      Locator::getLogger().log( Core::Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define WARN_RENDERING(...)     Locator::getLogger().warn( Core::Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define ERROR_RENDERING(...)    Locator::getLogger().error( Core::Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )

#define LOG_PHYSICS(...)        Locator::getLogger().log( Core::Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define WARN_PHYSICS(...)       Locator::getLogger().warn( Core::Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define ERROR_PHYSICS(...)      Locator::getLogger().error( Core::Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )

#define ASYNC_JOB(...)          Locator::getThreadManager().getThreadPool().addJob( __VA_ARGS__ )

#define PROFILER                Locator::getProfiler()
#define TIME                    Locator::getEngineClock()
#define SCENE                   Locator::getSceneManager().getCurrentScene()

//*********************************************************************
// Retrieve / Change every subsystem via a static method.
//*********************************************************************
class Locator
{
public:
    //----------------------------------------------------------------------
    // Retrieve a Sub-System
    //----------------------------------------------------------------------
    static Core::CoreEngine&                          getCoreEngine()     { return *gCoreEngine; }
    static Core::Logging::ILogger&                    getLogger()         { return *gLogger; }
    static Core::MemoryManagement::MemoryManager&     getMemoryManager()  { return *gMemoryManager; }
    static Core::Config::ConfigurationManager&        getConfiguration()  { return *gConfigManager; }
    static Core::Threading::ThreadManager&            getThreadManager()  { return *gThreadManager; }
    static Core::Profiling::Profiler&                 getProfiler()       { return *gProfiler; }
    static Core::Time::MasterClock&                   getEngineClock()    { return *gEngineClock; }
    static Core::OS::Window&                          getWindow()         { return *gWindow; }
    static Core::Input::InputManager&                 getInputManager()   { return *gInputManager; }
    static Core::IInGameConsole&                      getInGameConsole()  { return *gInGameConsole; }
    static Core::Graphics::IRenderer&                 getRenderer()       { return *gRenderer; }
    static Core::SceneManager&                        getSceneManager()   { return *gSceneManager; }
    static Core::Resources::ResourceManager&          getResourceManager(){ return *gResourceManager; }

    //----------------------------------------------------------------------
    // Provide a Sub-System
    //----------------------------------------------------------------------
    static void setCoreEngine(Core::CoreEngine* coreEngine)                   { gCoreEngine = coreEngine; }
    static void provide(Core::Logging::ILogger* logger)                       { gLogger = (logger != nullptr) ? logger : &gNullLogger; }
    static void provide(Core::MemoryManagement::MemoryManager* memoryManager) { gMemoryManager = memoryManager; }
    static void provide(Core::Config::ConfigurationManager* manager)          { gConfigManager = manager; }
    static void provide(Core::Threading::ThreadManager* manager)              { gThreadManager = manager; }
    static void provide(Core::Profiling::Profiler* profiler)                  { gProfiler = profiler; }
    static void provide(Core::Time::MasterClock* clock)                       { gEngineClock = clock; }
    static void provide(Core::OS::Window* window)                             { gWindow = window; }
    static void provide(Core::Input::InputManager* input)                     { gInputManager = input;  }
    static void provide(Core::IInGameConsole* igc)                            { gInGameConsole = igc; }
    static void provide(Core::Graphics::IRenderer* r)                         { gRenderer = r; }
    static void provide(Core::SceneManager* s)                                { gSceneManager = s; }
    static void provide(Core::Resources::ResourceManager* r)                  { gResourceManager = r; }

private:
    static Core::CoreEngine*                          gCoreEngine;

    //----------------------------------------------------------------------
    // All Sub-Systems are enumerated here
    //----------------------------------------------------------------------
    static Core::Logging::NullLogger                  gNullLogger;
    static Core::Logging::ILogger*                    gLogger;

    static Core::MemoryManagement::MemoryManager*     gMemoryManager;
    static Core::Config::ConfigurationManager*        gConfigManager;
    static Core::Threading::ThreadManager*            gThreadManager;
    static Core::Profiling::Profiler*                 gProfiler;
    static Core::Time::MasterClock*                   gEngineClock;
    static Core::OS::Window*                          gWindow;
    static Core::Input::InputManager*                 gInputManager;
    static Core::IInGameConsole*                      gInGameConsole;
    static Core::Graphics::IRenderer*                 gRenderer;
    static Core::SceneManager*                        gSceneManager;
    static Core::Resources::ResourceManager*          gResourceManager;

    //----------------------------------------------------------------------
    Locator()                                   = delete;
    Locator(const Locator& other)               = delete;
    Locator& operator = (const Locator& other)  = delete;
    Locator(Locator&& other)                    = delete;
    Locator& operator = (Locator&& other)       = delete;
};
