#pragma once

/**********************************************************************
    class: Logger (logger.h)

    author: S. Hau
    date: October 10, 2017

    Features:
     - Basic logging functionality to the console

**********************************************************************/

#include "Console/console.h"


//----------------------------------------------------------------------
#define LOG(x)              Core::Logging::gLogger.log(x)
#define LOG_NO_NEWLINE(x)   Core::Logging::gLogger.log(x)
#define WARN(x)             Core::Logging::gLogger.log(x)
#define ERROR(x)            Core::Logging::gLogger.log(x)


namespace Core {  namespace Logging  {

    //----------------------------------------------------------------------
    enum LOGTYPE
    {
        LOGTYPE_DEFAULT,
        LOGTYPE_WARNING,
        LOGTYPE_ERROR
    };

    //**********************************************************************
    // 
    //**********************************************************************
    class Logger
    {
    public:
        //----------------------------------------------------------------------
        // 
        //----------------------------------------------------------------------
        void log(const char* msg);
        void log(StringID msg);
        void log(void* addr);
        void log(String msg);
        void log(I64 num);

    private:
        Console console;

    };

    static Logger gLogger;

 } }