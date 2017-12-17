#include "locator.h"

/**********************************************************************
    class: Locator (locator.cpp)

    author: S. Hau
    date: October 11, 2017
**********************************************************************/

//----------------------------------------------------------------------
Core::CoreEngine*                       Locator::gCoreEngine;
Core::Logging::NullLogger               Locator::gNullLogger;
Core::Logging::ILogger*                 Locator::gLogger = &gNullLogger;

Core::MemoryManagement::MemoryManager*  Locator::gMemoryManager = nullptr;
Core::Config::ConfigurationManager*     Locator::gConfigManager = nullptr;
Core::Threading::ThreadManager*         Locator::gThreadManager = nullptr;
Core::Profiling::Profiler*              Locator::gProfiler      = nullptr;
Core::Time::MasterClock*                Locator::gEngineClock   = nullptr;
Core::OS::Window*                       Locator::gWindow        = nullptr;
Core::Input::InputManager*              Locator::gInputManager  = nullptr;
Core::IInGameConsole*                   Locator::gInGameConsole = nullptr;
Core::Graphics::IRenderer*              Locator::gRenderer      = nullptr;
Core::SceneManager*                     Locator::gSceneManager  = nullptr;