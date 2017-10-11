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
        void log(const char* msg) const override {};
        void log(StringID msg) const override {};
        void log(void* addr) const override {};
        void log(String msg) const override {};
        void log(I64 num) const override {};

    private:
        NullLogger(const NullLogger& other)                 = delete;
        NullLogger& operator = (const NullLogger& other)    = delete;
        NullLogger(NullLogger&& other)                      = delete;
        NullLogger& operator = (NullLogger&& other)         = delete;
    };



} } // end namespaces