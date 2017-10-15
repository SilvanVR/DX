#pragma once

/**********************************************************************
    class: ConsoleLogger (console_logger.h)

    author: S. Hau
    date: October 10, 2017

    See below for a class description.
**********************************************************************/

#include "i_logger.hpp"
#include "Console/console.h"

#include "Core/DataStructures/byte_array.hpp"

namespace Core {  namespace Logging  {

    //**********************************************************************
    // Basic logging functionality to the console
    //**********************************************************************
    class ConsoleLogger : public ILogger
    {
        static const U32 MSG_BUFFER_CAPACITY_BYTES = 1024;

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
        Console                                 m_console;
        ByteArray<MSG_BUFFER_CAPACITY_BYTES>    m_messageBuffer;

        //----------------------------------------------------------------------

        // Write messages in MessageBuffer to disk
        void _DumpToDisk();

        // Store a message as desired in the message buffer byte array
        void _StoreLogMessage(ELogChannel channel, const char* msg, ELogLevel logLevel);

        // Small helper function to write an character into the message buffer
        void _WriteToBuffer(const char* msg);


        ConsoleLogger(const ConsoleLogger& other)               = delete;
        ConsoleLogger& operator = (const ConsoleLogger& other)  = delete;
        ConsoleLogger(ConsoleLogger&& other)                    = delete;
        ConsoleLogger& operator = (ConsoleLogger&& other)       = delete;
    };

 } }