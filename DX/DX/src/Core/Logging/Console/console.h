#pragma once

/**********************************************************************
    class: Console (console.h)

    author: S. Hau
    date: October 10, 2017

    Platform independant encapsulation of the console window.
**********************************************************************/

#include "Core/Misc/color.h"

namespace Core { namespace Logging {

    class Console
    {
        //static const Color defaultColor = Color::WHITE;

    public:
        Console() {}

        //----------------------------------------------------------------------
        // Write the given string to the console with a new-line
        //----------------------------------------------------------------------
        void writeln(const char* msg) const;

        //----------------------------------------------------------------------
        // Write the string to the console without a new-line
        //----------------------------------------------------------------------
        void write(const char* msg) const;

        //----------------------------------------------------------------------
        // Flush the console buffer
        //----------------------------------------------------------------------
        void flush() const;

        //----------------------------------------------------------------------
        // Changes the text- and/or background color of the console window.
        // Note that the console does not support a wide variety of colors.
        // @Params:
        // "textColor": Color of the text.
        // "backgroundColor": Background color of the text.
        //----------------------------------------------------------------------
        void setColor(Color textColor, Color backgroundColor = Color::BLACK);

    private:
        Console(const Console& other)               = delete;
        Console& operator = (const Console& other)  = delete;
        Console(Console&& other)                    = delete;
        Console& operator = (Console&& other)       = delete;
    };


} } // end namespaces