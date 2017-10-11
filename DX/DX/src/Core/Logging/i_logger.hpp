#pragma once

/**********************************************************************
    class: ILogger (i_logger.hpp)

    author: S. Hau
    date: October 11, 2017

    Interface for a logger subsystem.
**********************************************************************/


#include "Core/i_subsystem.hpp"


namespace Core { namespace Logging {


    //----------------------------------------------------------------------
    enum LOGTYPE
    {
        LOGTYPE_DEFAULT,
        LOGTYPE_WARNING,
        LOGTYPE_ERROR
    };


    //**********************************************************************
    // Interface-Class for a Logging-Subsystem
    //**********************************************************************
    class ILogger : public ISubSystem
    {

    public:
        //----------------------------------------------------------------------
        // Log varies stuff to the console. X2
        //----------------------------------------------------------------------
        virtual void log(const char* msg) const = 0;
        virtual void log(StringID msg) const = 0;
        virtual void log(void* addr) const = 0;
        virtual void log(String msg) const = 0;
        virtual void log(I64 num) const = 0;

    };


} } // end namespaces
