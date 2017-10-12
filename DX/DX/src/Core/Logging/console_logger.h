#pragma once

/**********************************************************************
    class: ConsoleLogger (console_logger.h)

    author: S. Hau
    date: October 10, 2017

    Features:
     - Basic logging functionality to the console

**********************************************************************/

#include "i_logger.hpp"
#include "Console/console.h"

namespace Core {  namespace Logging  {


    //**********************************************************************
    // Basic logging functionality to the console
    //**********************************************************************
    class ConsoleLogger : public ILogger
    {
        const Color defaultColor = Color::WHITE;

    public:
        ConsoleLogger() {}

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // ILogger Interface
        //----------------------------------------------------------------------
        virtual void _Log(ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color) override;
        virtual void _Log(ELogChannel channel, const char* msg, Color color) override;

        virtual void _Warn(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override;
        virtual void _Error(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override;

    private:
        Console m_console;

        //----------------------------------------------------------------------
        // Write all LogMessage to the given file
        //----------------------------------------------------------------------
        void _WriteToFile(const char* fileName, bool append = false);

        //----------------------------------------------------------------------
        // Store the given message and flush the buffer if necessary
        //----------------------------------------------------------------------
        void _StoreLogMessage(ELogChannel channel, const char* msg, ELogLevel logLevel);

        ConsoleLogger(const ConsoleLogger& other)               = delete;
        ConsoleLogger& operator = (const ConsoleLogger& other)  = delete;
        ConsoleLogger(ConsoleLogger&& other)                    = delete;
        ConsoleLogger& operator = (ConsoleLogger&& other)       = delete;
    };

 } }