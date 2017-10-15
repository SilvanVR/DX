#pragma once

/**********************************************************************
    class: ConsoleLogger (console_logger.h)

    author: S. Hau
    date: October 10, 2017

    See below for a class description.
**********************************************************************/

#include "i_logger.hpp"
#include "Console/console.h"

namespace Core {  namespace Logging  {

    //**********************************************************************
    // Basic logging functionality to the console
    //**********************************************************************
    class ConsoleLogger : public ILogger
    {
        static const U32 MESSAGE_BUFFER_CAPACITY = 100;

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

        // Write messages in MessageBuffer to disk
        void _DumpToDisk() override;

    private:
        Console                     m_console;
        std::vector<_LOGMESSAGE>    m_messageBuffer;

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