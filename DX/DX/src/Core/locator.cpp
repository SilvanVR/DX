#include "locator.h"

/**********************************************************************
    class: Locator (locator.cpp)

    author: S. Hau
    date: October 11, 2017

**********************************************************************/

namespace Core {

    //----------------------------------------------------------------------
    // Define Static Instances
    //----------------------------------------------------------------------
    Logging::NullLogger                 Locator::gNullLogger;
    Logging::ILogger*                   Locator::gLogger = &gNullLogger;

    MemoryManagement::MemoryManager*    Locator::gMemoryManager = nullptr;
    Config::ConfigurationManager*       Locator::gConfigManager = nullptr;
    Threading::ThreadManager*           Locator::gThreadManager = nullptr;

}