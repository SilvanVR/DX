#include "Logging/logging.h"
/**********************************************************************
    class: None (logger.cpp)

    author: S. Hau
    date: February 27, 2018
**********************************************************************/

static Logging::NullLogger gNullLogger;
Logging::ILogger* gLogger = &gNullLogger;


namespace Logging {


    void ResetToNullLogger()
    {
        gLogger = &gNullLogger;
    }

}