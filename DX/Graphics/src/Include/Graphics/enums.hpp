#pragma once
/**********************************************************************
    class: None (enums.hpp)

    author: S. Hau
    date: March 7, 2018

    Contains definitions for important enums within a graphics
    application.
**********************************************************************/


namespace Graphics {

    // Available index-formats for an index buffer
    enum class IndexFormat
    {
        U16,
        U32
    };

    // Determines buffer types / update mechanism
    enum class BufferUsage
    {
        IMMUTABLE,      // Can't be updated once created.
        LONG_LIVED,     // Expected to be updated occasionally.
        FREQUENTLY      // Expected to be updated every frame.
    };

}
