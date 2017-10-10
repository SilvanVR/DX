#include "console.h"

/**********************************************************************
    class: Console (console.cpp)

    author: S. Hau
    date: October 10, 2017

    Platform independant implementation of the console.

**********************************************************************/

#include <iostream>

namespace Core { namespace Logging {

    //----------------------------------------------------------------------
    void Console::writeln(const char* msg)
    {
        std::cout << msg << std::endl;
    }

    //----------------------------------------------------------------------
    void Console::write(const char* msg)
    {
        std::cout << msg;
    }

#ifndef _WIN32

    //----------------------------------------------------------------------
    void Console::setTextColor(Color color)
    {
        static_assert(false, "Function not implemented on this platform. Either uncomment this line or implement it");
    }

#endif


} } // end namespaces