#pragma once
/**********************************************************************
    class: Rect (rect.h)

    author: S. Hau
    date: June 20, 2018
**********************************************************************/

namespace Math
{

    //**********************************************************************
    template <typename T>
    struct RectT
    {
        T left    = 0.0f;
        T top     = 0.0f;
        T right   = 0.0f;
        T bottom  = 0.0f;

        String toString() { return "Left: " + TS(left) + " Right: " + TS(right) + " Bottom: " + TS(bottom) + " Top: " + TS(top); }
    };

    using Rect = RectT<I32>;

}