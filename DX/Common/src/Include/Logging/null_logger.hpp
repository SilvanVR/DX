#pragma once

/**********************************************************************
    class: NullLogger (null_logger.hpp)

    author: S. Hau
    date: October 11, 2017

    Logger which does nothing.
**********************************************************************/

#include "i_logger.hpp"

namespace Logging {


    class NullLogger : public ILogger
    {

    public:
        NullLogger() {}

        virtual void _Log(ELogChannel channel, const char* msg, ELogLevel ELogLevel, Color color) override {}
        virtual void _Log(ELogChannel channel, const char* msg, Color color) override {}

        virtual void _Warn(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override {}
        virtual void _Error(ELogChannel channel, const char* msg, ELogLevel ELogLevel) override {}

    private:
        NullLogger(const NullLogger& other)                 = delete;
        NullLogger& operator = (const NullLogger& other)    = delete;
        NullLogger(NullLogger&& other)                      = delete;
        NullLogger& operator = (NullLogger&& other)         = delete;
    };



} // end namespaces