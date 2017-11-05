#include "locator.h"

/**********************************************************************
    class: Locator (locator.cpp)

    author: S. Hau
    date: October 11, 2017

**********************************************************************/


//----------------------------------------------------------------------
// Define Static Instances
//----------------------------------------------------------------------
Core::Logging::NullLogger                 Locator::gNullLogger;
Core::Logging::ILogger*                   Locator::gLogger = &gNullLogger;

Core::MemoryManagement::MemoryManager*    Locator::gMemoryManager = nullptr;
Core::Config::ConfigurationManager*       Locator::gConfigManager = nullptr;
Core::Threading::ThreadManager*           Locator::gThreadManager = nullptr;
Core::Profiling::Profiler*                Locator::gProfiler      = nullptr;
Core::Time::MasterClock*                  Locator::gEngineClock   = nullptr;
Core::OS::Window*                         Locator::gWindow        = nullptr;
Core::Input::InputManager*                Locator::gInputManager  = nullptr;