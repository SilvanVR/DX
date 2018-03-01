#pragma once
/**********************************************************************
    class: None (logging.hpp)

    author: S. Hau
    date: February 27, 2018

    Include file for using the logging mechanism.
**********************************************************************/

#include "Logging/shared_console_logger.hpp"

extern Logging::SharedConsoleLogger gLogger;

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                gLogger.log( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define WARN(...)               gLogger.warn( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define ERROR(...)              gLogger.error( Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )

#define LOG_TEST(...)           gLogger.log( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )
#define WARN_TEST(...)          gLogger.warn( Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )

#define LOG_MEMORY(...)         gLogger.log( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define WARN_MEMORY(...)        gLogger.warn( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define ERROR_MEMORY(...)       gLogger.error( Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )

#define LOG_RENDERING(...)      gLogger.log( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define WARN_RENDERING(...)     gLogger.warn( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define ERROR_RENDERING(...)    gLogger.error( Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )

#define LOG_PHYSICS(...)        gLogger.log( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define WARN_PHYSICS(...)       gLogger.warn( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define ERROR_PHYSICS(...)      gLogger.error( Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )