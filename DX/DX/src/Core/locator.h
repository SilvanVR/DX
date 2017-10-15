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

#include "Logging/null_logger.hpp"
#include "MemoryManagement/memory_manager.h"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                Core::Locator::getLogger().log( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define WARN(...)               Core::Locator::getLogger().warn( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define ERROR(...)              Core::Locator::getLogger().error( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )

#define LOG_TEST(...)           Core::Locator::getLogger().log( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )
#define WARN_TEST(...)          Core::Locator::getLogger().warn( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )

#define LOG_MEMORY(...)         Core::Locator::getLogger().log( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define WARN_MEMORY(...)        Core::Locator::getLogger().warn( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define ERROR_MEMORY(...)       Core::Locator::getLogger().error( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )

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
        static Logging::ILogger&                    getLogger();
        static MemoryManagement::MemoryManager&     getMemoryManager();

        //----------------------------------------------------------------------
        // Provide a Sub-System
        //----------------------------------------------------------------------
        static void provide(Logging::ILogger* logger) { gLogger = (logger != nullptr) ? logger : &gNullLogger; }
        static void provide(MemoryManagement::MemoryManager* memoryManager){ gMemoryManager = memoryManager; }

    private:

        //----------------------------------------------------------------------
        // All Sub-Systems are enumerated here
        //----------------------------------------------------------------------
        static Logging::NullLogger                  gNullLogger;
        static Logging::ILogger*                    gLogger;

        static MemoryManagement::MemoryManager*     gMemoryManager;


        // Do not allow construction of an Locator-Object
        Locator()                                   = delete;
        Locator(const Locator& other)               = delete;
        Locator& operator = (const Locator& other)  = delete;
        Locator(Locator&& other)                    = delete;
        Locator& operator = (Locator&& other)       = delete;
    };

} // end namespaces