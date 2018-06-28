#pragma once
/**********************************************************************
    class: None (layers.hpp)

    author: S. Hau
    date: April 28, 2018
**********************************************************************/

enum class Layer
{
    None = 0,
    One             = 1 << 0,
    Two             = 1 << 1,
    Three           = 1 << 2,
    Four            = 1 << 3,
    Five            = 1 << 4,
    Six             = 1 << 5,
    Seven           = 1 << 6,
    Eight           = 1 << 7,
    IgnoreRaycasts  = 1 << 24,
    All = ~0
};

using LayerMask = U32;

#define LAYER_DEFAULT   (LayerMask)Layer::One
#define LAYER_ALL       (LayerMask)Layer::All

constexpr LayerMask operator & ( Layer e1, Layer e2 )
{
    return static_cast<LayerMask>(e1) & static_cast<LayerMask>(e2);
}

constexpr LayerMask operator | ( Layer e1, Layer e2 )
{
    return static_cast<LayerMask>(e1) | static_cast<LayerMask>(e2);
}

constexpr LayerMask operator & (LayerMask e1, Layer e2)
{
    return static_cast<LayerMask>(e1) & static_cast<LayerMask>(e2);
}

constexpr LayerMask operator | (LayerMask e1, Layer e2)
{
    return static_cast<LayerMask>(e1) | static_cast<LayerMask>(e2);
}

constexpr LayerMask& operator |= (LayerMask& e1, Layer e2)
{
    e1 = (e1 | e2);
    return e1;
}

constexpr LayerMask& operator &= (LayerMask& e1, Layer e2)
{
    e1 = (e1 & e2);
    return e1;
}