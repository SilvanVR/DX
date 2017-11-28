#pragma once

/**********************************************************************
    class: SharedConsoleLogger (shared_console_logger.hpp)

    author: S. Hau
    date: October 22, 2017

    Thread safe shared access to the console logger.
**********************************************************************/

#include "console_logger.h"

namespace Core {  namespace Logging  {

    //**********************************************************************
    // Safe logging for multiple threads.
    //**********************************************************************
    class SharedConsoleLogger : public ConsoleLogger
    {
    public:
        SharedConsoleLogger() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init(CoreEngine* coreEngine) override { ConsoleLogger::init( coreEngine ); }
        void shutdown() override { ConsoleLogger::shutdown(); }

        //----------------------------------------------------------------------
        // ILogger Interface
        //----------------------------------------------------------------------
        void _Log(ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color) override
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            ConsoleLogger::_Log( channel, msg, ELogLevel, color );
        }

        void _Log(ELogChannel channel, const char* msg, Color color) override
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            ConsoleLogger::_Log( channel, msg, color );
        }

        void _Warn(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            ConsoleLogger::_Warn( channel, msg, ELogLevel );
        }

        void _Error(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            ConsoleLogger::_Error( channel, msg, ELogLevel );
        }

    private:
        std::mutex m_mutex;

        SharedConsoleLogger(const SharedConsoleLogger& other)               = delete;
        SharedConsoleLogger& operator = (const SharedConsoleLogger& other)  = delete;
        SharedConsoleLogger(SharedConsoleLogger&& other)                    = delete;
        SharedConsoleLogger& operator = (SharedConsoleLogger&& other)       = delete;
    };

 } }
