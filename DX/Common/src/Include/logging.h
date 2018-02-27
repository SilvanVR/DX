#pragma once
/**********************************************************************
    class: None (logging.hpp)

    author: S. Hau
    date: February 27, 2018

    Include file for using the logging mechanism.
**********************************************************************/

#include "Logging/shared_console_logger.hpp"

extern Core::Logging::SharedConsoleLogger gLogger;

//----------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------
#define LOG(...)                gLogger.log( Core::Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define WARN(...)               gLogger.warn( Core::Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )
#define ERROR(...)              gLogger.error( Core::Logging::LOG_CHANNEL_DEFAULT, __VA_ARGS__ )

#define LOG_TEST(...)           gLogger.log( Core::Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )
#define WARN_TEST(...)          gLogger.warn( Core::Logging::LOG_CHANNEL_TEST, __VA_ARGS__ )

#define LOG_MEMORY(...)         gLogger.log( Core::Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define WARN_MEMORY(...)        gLogger.warn( Core::Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )
#define ERROR_MEMORY(...)       gLogger.error( Core::Logging::LOG_CHANNEL_MEMORY, __VA_ARGS__ )

#define LOG_RENDERING(...)      gLogger.log( Core::Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define WARN_RENDERING(...)     gLogger.warn( Core::Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )
#define ERROR_RENDERING(...)    gLogger.error( Core::Logging::LOG_CHANNEL_RENDERING, __VA_ARGS__ )

#define LOG_PHYSICS(...)        gLogger.log( Core::Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define WARN_PHYSICS(...)       gLogger.warn( Core::Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )
#define ERROR_PHYSICS(...)      gLogger.error( Core::Logging::LOG_CHANNEL_PHYSICS, __VA_ARGS__ )