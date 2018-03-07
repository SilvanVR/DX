#pragma once
/**********************************************************************
    class: None (structs.hpp)

    author: S. Hau
    date: March 7, 2018

    Contains definitions for important structures within a graphics
    application.
**********************************************************************/


namespace Graphics {

    // Coordinates specified in [0-1] Range
    struct ViewportRect
    {
        F32 topLeftX    = 0.0f;
        F32 topLeftY    = 0.0f;
        F32 width       = 1.0f;
        F32 height      = 1.0f;
    };

}
