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

#include "MemoryManager/memory_manager.h"
#include "Config/configuration_manager.h"
#include "ThreadManager/thread_manager.h"
#include "Profiling/profiler.h"
#include "Time/master_clock.h"
#include "OS/Window/window.h"
#include "Input/input_manager.h"
#include "core_engine.h"
#include "InGameConsole/i_in_game_console.hpp"
#include "Graphics/i_renderer.h"
#include "SceneManager/scene_manager.h"
#include "Resources/resource_manager.h"
#include "DebugManager/debug_manager.h"
#include "Assets/asset_manager.h"
#include "Audio/audio_manager.h"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define ASYNC_JOB(...)          Locator::getThreadManager().getThreadPool().addJob( __VA_ARGS__ )

#define PROFILER                Locator::getProfiler()
#define TIME                    Locator::getEngineClock()
#define SCENE                   Locator::getSceneManager().getCurrentScene()
#define RESOURCES               Locator::getResourceManager()
#define DEBUG                   Locator::getDebugManager()
#define ASSETS                  Locator::getAssetManager()
#define WINDOW                  Locator::getWindow()

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
    static Core::MemoryManagement::MemoryManager&     getMemoryManager()  { return *gMemoryManager; }
    static Core::Config::ConfigurationManager&        getConfiguration()  { return *gConfigManager; }
    static Core::Threading::ThreadManager&            getThreadManager()  { return *gThreadManager; }
    static Core::Profiling::Profiler&                 getProfiler()       { return *gProfiler; }
    static Time::MasterClock&                         getEngineClock()    { return *gEngineClock; }
    static OS::Window&                                getWindow()         { return *gWindow; }
    static Core::Input::InputManager&                 getInputManager()   { return *gInputManager; }
    static Core::IInGameConsole&                      getInGameConsole()  { return *gInGameConsole; }
    static Graphics::IRenderer&                       getRenderer()       { return *gRenderer; }
    static Core::SceneManager&                        getSceneManager()   { return *gSceneManager; }
    static Core::Resources::ResourceManager&          getResourceManager(){ return *gResourceManager; }
    static Core::Debug::DebugManager&                 getDebugManager()   { return *gDebugManager;}
    static Assets::AssetManager&                      getAssetManager()   { return *gAssetManager; }
    static Core::Audio::AudioManager&                 getAudioManager()   { return *gAudioManager; }

    //----------------------------------------------------------------------
    // Provide a Sub-System
    //----------------------------------------------------------------------
    static void setCoreEngine(Core::CoreEngine* coreEngine)                   { gCoreEngine = coreEngine; }
    static void provide(Core::MemoryManagement::MemoryManager* memoryManager) { gMemoryManager = memoryManager; }
    static void provide(Core::Config::ConfigurationManager* manager)          { gConfigManager = manager; }
    static void provide(Core::Threading::ThreadManager* manager)              { gThreadManager = manager; }
    static void provide(Core::Profiling::Profiler* profiler)                  { gProfiler = profiler; }
    static void provide(Time::MasterClock* clock)                             { gEngineClock = clock; }
    static void provide(OS::Window* window)                                   { gWindow = window; }
    static void provide(Core::Input::InputManager* input)                     { gInputManager = input;  }
    static void provide(Core::IInGameConsole* igc)                            { gInGameConsole = igc; }
    static void provide(Graphics::IRenderer* r)                               { gRenderer = r; }
    static void provide(Core::SceneManager* s)                                { gSceneManager = s; }
    static void provide(Core::Resources::ResourceManager* r)                  { gResourceManager = r; }
    static void provide(Core::Debug::DebugManager* d)                         { gDebugManager = d; }
    static void provide(Assets::AssetManager* a)                              { gAssetManager = a; }
    static void provide(Core::Audio::AudioManager* a)                         { gAudioManager = a; }

private:
    static Core::CoreEngine*                          gCoreEngine;

    //----------------------------------------------------------------------
    // All Sub-Systems are enumerated here
    //----------------------------------------------------------------------
    static Core::MemoryManagement::MemoryManager*     gMemoryManager;
    static Core::Config::ConfigurationManager*        gConfigManager;
    static Core::Threading::ThreadManager*            gThreadManager;
    static Core::Profiling::Profiler*                 gProfiler;
    static Time::MasterClock*                         gEngineClock;
    static OS::Window*                                gWindow;
    static Core::Input::InputManager*                 gInputManager;
    static Core::IInGameConsole*                      gInGameConsole;
    static Graphics::IRenderer*                       gRenderer;
    static Core::SceneManager*                        gSceneManager;
    static Core::Resources::ResourceManager*          gResourceManager;
    static Core::Debug::DebugManager*                 gDebugManager;
    static Assets::AssetManager*                      gAssetManager;
    static Core::Audio::AudioManager*                 gAudioManager;

    //----------------------------------------------------------------------
    Locator() = delete;
};
