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
#include "Config/configuration_manager.h"
#include "ThreadManager/thread_manager.h"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                Core::Locator::getLogger().log( Logging::ELogChannel::DEFAULT, __VA_ARGS__ )
#define WARN(...)               Core::Locator::getLogger().warn( Logging::ELogChannel::DEFAULT, __VA_ARGS__ )
#define ERROR(...)              Core::Locator::getLogger().error( Logging::ELogChannel::DEFAULT, __VA_ARGS__ )

#define LOG_TEST(...)           Core::Locator::getLogger().log( Logging::ELogChannel::TEST, __VA_ARGS__ )
#define WARN_TEST(...)          Core::Locator::getLogger().warn( Logging::ELogChannel::TEST, __VA_ARGS__ )

#define LOG_MEMORY(...)         Core::Locator::getLogger().log( Logging::ELogChannel::MEMORY, __VA_ARGS__ )
#define WARN_MEMORY(...)        Core::Locator::getLogger().warn( Logging::ELogChannel::MEMORY, __VA_ARGS__ )
#define ERROR_MEMORY(...)       Core::Locator::getLogger().error( Logging::ELogChannel::MEMORY, __VA_ARGS__ )

#define LOG_RENDERING(...)      Core::Locator::getLogger().log( Logging::ELogChannel::RENDERING, __VA_ARGS__ )
#define WARN_RENDERING(...)     Core::Locator::getLogger().warn( Logging::ELogChannel::RENDERING, __VA_ARGS__ )
#define ERROR_RENDERING(...)    Core::Locator::getLogger().error( Logging::ELogChannel::RENDERING, __VA_ARGS__ )

#define LOG_PHYSICS(...)        Core::Locator::getLogger().log( Logging::ELogChannel::PHYSICS, __VA_ARGS__ )
#define WARN_PHYSICS(...)       Core::Locator::getLogger().warn( Logging::ELogChannel::PHYSICS, __VA_ARGS__ )
#define ERROR_PHYSICS(...)      Core::Locator::getLogger().error( Logging::ELogChannel::PHYSICS, __VA_ARGS__ )

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
        static Logging::ILogger&                    getLogger()         { return *gLogger; }
        static MemoryManagement::MemoryManager&     getMemoryManager()  { return *gMemoryManager; }
        static Config::ConfigurationManager&        getConfiguration()  { return *gConfigManager; }
        static Threading::ThreadManager&            getThreadManager()  { return *gThreadManager; }

        //----------------------------------------------------------------------
        // Provide a Sub-System
        //----------------------------------------------------------------------
        static void provide(Logging::ILogger* logger)                       { gLogger = (logger != nullptr) ? logger : &gNullLogger; }
        static void provide(MemoryManagement::MemoryManager* memoryManager) { gMemoryManager = memoryManager; }
        static void provide(Config::ConfigurationManager* manager)          { gConfigManager = manager; }
        static void provide(Threading::ThreadManager* manager)              { gThreadManager = manager; }

    private:

        //----------------------------------------------------------------------
        // All Sub-Systems are enumerated here
        //----------------------------------------------------------------------
        static Logging::NullLogger                  gNullLogger;
        static Logging::ILogger*                    gLogger;

        static MemoryManagement::MemoryManager*     gMemoryManager;
        static Config::ConfigurationManager*        gConfigManager;
        static Threading::ThreadManager*            gThreadManager;


        // Do not allow construction of an Locator-Object
        Locator()                                   = delete;
        Locator(const Locator& other)               = delete;
        Locator& operator = (const Locator& other)  = delete;
        Locator(Locator&& other)                    = delete;
        Locator& operator = (Locator&& other)       = delete;
    };

} // end namespaces