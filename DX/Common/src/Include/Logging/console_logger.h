#pragma once

/**********************************************************************
    class: ConsoleLogger (console_logger.h)

    author: S. Hau
    date: October 10, 2017

    See below for a class description.
**********************************************************************/

#include "i_logger.hpp"
#include "Console/console.h"
#include "Common/DataStructures/byte_array.hpp"

namespace Logging  {

    //**********************************************************************
    // Basic logging functionality to the console
    //**********************************************************************
    class ConsoleLogger : public ILogger
    {
        static const U32 MSG_BUFFER_CAPACITY_BYTES = 2048;

    public:
        ConsoleLogger();
        virtual ~ConsoleLogger();

        //----------------------------------------------------------------------
        // ILogger Interface
        //----------------------------------------------------------------------
        virtual void _Log(ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color) override;
        virtual void _Log(ELogChannel channel, const char* msg, Color color) override;

        virtual void _Warn(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override;
        virtual void _Error(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override;

    private:
        Console                                         m_console;
        Common::ByteArray<MSG_BUFFER_CAPACITY_BYTES>    m_messageBuffer;
        String                                          m_logFilePath;

        //----------------------------------------------------------------------
        void _LOG( ELogType type, ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color );

        // Write messages in MessageBuffer to disk
        void _DumpToDisk();

        // Store a message as desired in the message buffer byte array
        void _StoreLogMessage(ELogType logType, ELogChannel channel, const char* msg, ELogLevel logLevel);

        // Small helper function to write an character into the message buffer
        void _WriteToBuffer(const char* msg);


        ConsoleLogger(const ConsoleLogger& other)               = delete;
        ConsoleLogger& operator = (const ConsoleLogger& other)  = delete;
        ConsoleLogger(ConsoleLogger&& other)                    = delete;
        ConsoleLogger& operator = (ConsoleLogger&& other)       = delete;
    };

 }