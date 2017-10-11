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
    // 
    //**********************************************************************
    class ConsoleLogger : public ILogger
    {
    public:
        ConsoleLogger() {}

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void shutdown() override;

        //----------------------------------------------------------------------
        // Log varies stuff to the console.
        //----------------------------------------------------------------------
        void log(const char* msg) const override;
        void log(StringID msg) const override;
        void log(void* addr) const override;
        void log(String msg) const override;
        void log(I64 num) const override;

    private:
        Console m_console;

        ConsoleLogger(const ConsoleLogger& other)               = delete;
        ConsoleLogger& operator = (const ConsoleLogger& other)  = delete;
        ConsoleLogger(ConsoleLogger&& other)                    = delete;
        ConsoleLogger& operator = (ConsoleLogger&& other)       = delete;
    };

 } }