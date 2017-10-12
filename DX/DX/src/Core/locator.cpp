#include "locator.h"


namespace Core {


    //----------------------------------------------------------------------
    // Define Static Instances
    //----------------------------------------------------------------------
    Logging::NullLogger                 Locator::gNullLogger;
    Logging::ILogger*                   Locator::gLogger = &gNullLogger;

    MemoryManagement::MemoryManager*    Locator::gMemoryManager;

}