#pragma once

/**********************************************************************
    class: NullLogger (null_logger.hpp)

    author: S. Hau
    date: October 11, 2017

    Logger which does nothing.
**********************************************************************/

#include "i_logger.hpp"

namespace Core { namespace Logging {


    class NullLogger : public ILogger
    {

    public:
        NullLogger() {}

        void init() override {}
        void shutdown() override {}

        virtual void _Log(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel, Color color) const override {}
        virtual void _Log(LOGCHANNEL channel, const char* msg, Color color) const override {}

        virtual void _Warn(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const override {}
        virtual void _Error(LOGCHANNEL channel, const char* msg, LOGLEVEL logLevel) const override {}

    private:
        NullLogger(const NullLogger& other)                 = delete;
        NullLogger& operator = (const NullLogger& other)    = delete;
        NullLogger(NullLogger&& other)                      = delete;
        NullLogger& operator = (NullLogger&& other)         = delete;
    };



} } // end namespaces