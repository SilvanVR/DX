#pragma once
/**********************************************************************
    class: None (math_utils.h)
    
    author: S. Hau
    date: March 7, 2018

    Common utility functions for math stuff.
**********************************************************************/

namespace Math {

    //----------------------------------------------------------------------
    template <typename T> inline
    float deg2Rad( const T& val )
    {
        return val * 0.01745329251f;
    }

    //----------------------------------------------------------------------
    template <typename T> inline
    float rad2Deg( const T& val )
    {
        return val * 57.2957795131f;
    }

}