#pragma once
/**********************************************************************
    class: None (input_enums.hpp)

    author: S. Hau
    date: November 18, 2017

    Enum for all supported input devices and input channels.
    Every device and listener has a channel mask.
    If the master bit is set in the device all immediate function
    calls maintain their usual functionality (e.g. isKeyDown() works),
    otherwise every function return false.
**********************************************************************/

#include "Common/enum_class_operators.hpp"

namespace Core { namespace Input {

    //----------------------------------------------------------------------
    enum class EInputDevice
    {
        Keyboard,
        Mouse
    };

    //----------------------------------------------------------------------
    enum class EInputChannels
    {
        None    = 0,
        Master  = 1 << 0,   // Master channel. If this is 0 then no isKeyDown etc works on all input devices
        One     = 1 << 1,
        Two     = 1 << 2,
        Three   = 1 << 3,
        Four    = 1 << 4,
        Five    = 1 << 5,
        Six     = 1 << 6,
        Seven   = 1 << 7,
        Eight   = 1 << 8,
        Nine    = 1 << 9,
        Ten     = 1 << 10,
        GUI     = 1 << 29,  // GUI channel
        Console = 1 << 30,  // Console channel. Used exclusively for the console. Basically disables every input except for the console.
        Default = Master | One | Two | Three | Four | Five | Six | Seven | Eight | Nine | Ten,
        All     = ~0
    };
    ENABLE_BITMASK_OPERATORS(EInputChannels)

} }