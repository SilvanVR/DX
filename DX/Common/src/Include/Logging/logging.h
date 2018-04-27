#pragma once
/**********************************************************************
    class: None (logging.hpp)

    author: S. Hau
    date: February 27, 2018

    Include file for using the logging mechanism.
    The gLogger is initially initialized to a NullLogger, which 
    does nothing. So in order to use logging a suitable ILogger has
    to be initialized first and set to the global variable 
    e.g. gLogger = new ConsoleLogger(). If this logger gets 
    destroyed call "Logging::ResetToDefaultLogger()" to reset it back
    to the default logger, otherwise subsequent LOG calls will throw errors.
**********************************************************************/

#include "null_logger.hpp"

extern Logging::ILogger*            gLogger;

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                    gLogger->log( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define LOG_WARN(...)               gLogger->warn( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define LOG_ERROR(...)              gLogger->error( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )

#define LOG_TEST(...)               gLogger->log( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )
#define LOG_WARN_TEST(...)          gLogger->warn( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )
#define LOG_ERROR_TEST(...)         gLogger->error( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )

#define LOG_MEMORY(...)             gLogger->log( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define LOG_WARN_MEMORY(...)        gLogger->warn( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define LOG_ERROR_MEMORY(...)       gLogger->error( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )

#define LOG_RENDERING(...)          gLogger->log( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define LOG_WARN_RENDERING(...)     gLogger->warn( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define LOG_ERROR_RENDERING(...)    gLogger->error( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )

#define LOG_PHYSICS(...)            gLogger->log( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define LOG_WARN_PHYSICS(...)       gLogger->warn( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define LOG_ERROR_PHYSICS(...)      gLogger->error( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )

#define LOG_AUDIO(...)              gLogger->log( Logging::LOG_CHANNEL_AUDIO, __VA_ARGS__ )
#define LOG_WARN_AUDIO(...)         gLogger->warn( Logging::LOG_CHANNEL_AUDIO, __VA_ARGS__ )
#define LOG_ERROR_AUDIO(...)        gLogger->error( Logging::LOG_CHANNEL_AUDIO, __VA_ARGS__ )

namespace Logging {

    //----------------------------------------------------------------------
    // Sets the global-logger back to the null-logger (which does nothing).
    //----------------------------------------------------------------------
    void ResetToNullLogger();

}