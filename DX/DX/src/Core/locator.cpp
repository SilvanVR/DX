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

    //----------------------------------------------------------------------
    MemoryManagement::MemoryManager& Locator::getMemoryManager()
    {
        return (*gMemoryManager);
    }

    Logging::ILogger& Locator::getLogger() 
    { 
        return (*gLogger); 
    }

    Config::ConfigurationManager& Locator::getConfiguration()
    {
        return (*gConfigManager);
    }


}