#pragma once

/**********************************************************************
    class: Locator (locator.h)

    author: S. Hau
    date: October 11, 2017

    Implements the Service-Locator Pattern.
    Every Sub-System can be retrieved via a static method.

    Features:
        [+] Every Sub-System in one place
        [+] Exact retrieved Sub-System is only known at runtime
          [++] Every SubSystem can be easily swapped out
          [++] Hierarchies of a specific Sub-System are possible, e.g.
               Logged Audio!
        [+] Easy extensible
        [-] Interface must be known a priori
        [-] More code + files
          [--] Templates are harder to use
        [-] Less performance because of virtual functions
        [-] Every Sub-System in one place :)

**********************************************************************/

#include "Logging/null_logger.hpp"
#include "MemoryManagement/memory_manager.h"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                Core::Locator::getLogger().log( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define WARN(...)               Core::Locator::getLogger().warn( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define ERROR(...)              Core::Locator::getLogger().error( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )

#define LOG_RENDERING(...)      Core::Locator::getLogger().log( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define WARN_RENDERING(...)     Core::Locator::getLogger().warn( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define ERROR_RENDERING(...)    Core::Locator::getLogger().error( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )

#define LOG_PHYSICS(...)        Core::Locator::getLogger().log( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define WARN_PHYSICS(...)       Core::Locator::getLogger().warn( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define ERROR_PHYSICS(...)      Core::Locator::getLogger().error( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )

namespace Core {

    //*********************************************************************
    // Retrieve / Change every subsystem via a static method.
    //*********************************************************************
    class Locator
    {
    public:
        //----------------------------------------------------------------------
        // Retrieve a Sub-System
        //----------------------------------------------------------------------
        static Logging::ILogger& getLogger() { return (*gLogger); }

        static MemoryManagement::MemoryManager& getMemoryManager();

        //----------------------------------------------------------------------
        // Provide a Sub-System
        //----------------------------------------------------------------------
        static void provide(Logging::ILogger* logger) { gLogger = (logger != nullptr) ? logger : &gNullLogger; }
        //static void provide(Logging::ILogger* logger) { gLogger = logger; }

        static void provide(MemoryManagement::MemoryManager* memoryManager){ gMemoryManager = memoryManager; }

    private:

        //----------------------------------------------------------------------
        // All Sub-Systems are enumerated here
        //----------------------------------------------------------------------
        static Logging::NullLogger              gNullLogger;
        static Logging::ILogger*                gLogger;

        static MemoryManagement::MemoryManager* gMemoryManager;


        // Do not allow construction of an Locator-Object
        Locator()                                   = delete;
        Locator(const Locator& other)               = delete;
        Locator& operator = (const Locator& other)  = delete;
        Locator(Locator&& other)                    = delete;
        Locator& operator = (Locator&& other)       = delete;
    };

} // end namespaces