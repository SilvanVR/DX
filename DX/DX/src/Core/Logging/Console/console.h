#pragma once

/**********************************************************************
    class: Console (console.h)

    author: S. Hau
    date: October 10, 2017

    Platform independant encapsulation of the console window.
**********************************************************************/

#include "PrecompiledHeaders/color.h"

namespace Core { namespace Logging {

    class Console
    {
        const Color defaultColor = Color::WHITE;
        const Byte defaultFontSize = 24;

    public:
        void writeln(const char* msg);
        void write(const char* msg);

        //----------------------------------------------------------------------
        // Changes the text- and/or background color of the console window.
        // Note that the console does not support a wide variety of colors.
        // @Params:
        // "textColor": Color of the text.
        // "backgroundColor": Background color of the text.
        //----------------------------------------------------------------------
        void setColor(Color textColor, Color backgroundColor = Color::BLACK);

        //----------------------------------------------------------------------
        // 
        //----------------------------------------------------------------------
        void setFontSize();

    private:
    };


} } // end namespaces