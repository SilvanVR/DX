#include "console.h"

/**********************************************************************
    class: Console (console_win.cpp)

    author: S. Hau
    date: October 10, 2017

    Windows implementation of some console functions.

**********************************************************************/

#ifdef _WIN32

#include <windows.h>
#include <cwchar>

namespace Core { namespace Logging {


    //----------------------------------------------------------------------
    void Console::setColor( Color color, Color backgroundColor ) const
    {
        HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

        // Set Text-Color
        int col = (color.getMax() > 127 ? FOREGROUND_INTENSITY : 0);
        if (color.getRed() > 0)
            col |= FOREGROUND_RED;
        if (color.getGreen() > 0)
            col |= FOREGROUND_GREEN;
        if (color.getBlue() > 0)
            col |= FOREGROUND_BLUE;

        // Set Background-Color
        if (backgroundColor.getRed() > 0)
            col |= BACKGROUND_RED;
        if (backgroundColor.getGreen() > 0)
            col |= BACKGROUND_GREEN;
        if (backgroundColor.getBlue() > 0)
            col |= BACKGROUND_BLUE;
        if (backgroundColor.getMax() > 127)
            col |= BACKGROUND_INTENSITY;

        SetConsoleTextAttribute( hConsole, col );
    }


} } // end namespaces


#endif // !_WIN32