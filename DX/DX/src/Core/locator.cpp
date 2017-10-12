#include "locator.h"


#include "MemoryManagement/gnad_memory_manager.h"

namespace Core {


    //----------------------------------------------------------------------
    // Define Static Instances
    //----------------------------------------------------------------------
    Logging::NullLogger                 Locator::gNullLogger;
    Logging::ILogger*                   Locator::gLogger = &gNullLogger;

    MemoryManagement::MemoryManager*    Locator::gMemoryManager = nullptr;


    //----------------------------------------------------------------------
    MemoryManagement::MemoryManager& Locator::getMemoryManager()
    {
        return (*gMemoryManager);
    }

    Logging::ILogger& Locator::getLogger() 
    { 
        return (*gLogger); 
    }




}