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
        [-] Interface must be known a priori
        [-] More code + files
        [-] Every Sub-System in one place :)

**********************************************************************/

#include "Logging/null_logger.hpp"

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(x)              Core::Locator::getLogger().log(x)
#define LOG_NO_NEWLINE(x)   Core::Locator::getLogger().log(x)
#define WARN(x)             Core::Locator::getLogger().log(x)
#define ERROR(x)            Core::Locator::getLogger().log(x)


namespace Core {

    class Locator
    {
    public:
        //----------------------------------------------------------------------
        // Retrieve a Sub-System
        //----------------------------------------------------------------------
        static Logging::ILogger& getLogger() { return (*gLogger); }

        //----------------------------------------------------------------------
        // Set a Sub-System
        //----------------------------------------------------------------------
        static void provide(Logging::ILogger* logger) { gLogger = (logger != nullptr) ? logger : &gNullLogger; }

    private:

        //----------------------------------------------------------------------
        // All Sub-Systems are enumerated here
        //----------------------------------------------------------------------
        static Logging::ILogger*    gLogger;
        static Logging::NullLogger  gNullLogger;


        // Do not allow construction of an Locator-Object
        Locator()                                   = delete;
        Locator(const Locator& other)               = delete;
        Locator& operator = (const Locator& other)  = delete;
        Locator(Locator&& other)                    = delete;
        Locator& operator = (Locator&& other)       = delete;
    };

} // end namespaces