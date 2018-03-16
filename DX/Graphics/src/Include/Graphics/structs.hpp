#pragma once
/**********************************************************************
    class: None (structs.hpp)

    author: S. Hau
    date: March 7, 2018

    Contains definitions for important structures within a graphics
    application.
**********************************************************************/

#include "enums.hpp"

namespace Graphics {

    // Coordinates specified in [0-1] Range
    struct ViewportRect
    {
        F32 topLeftX    = 0.0f;
        F32 topLeftY    = 0.0f;
        F32 width       = 1.0f;
        F32 height      = 1.0f;
    };

    // Rasterizer State
    struct RasterizationState
    {
        FillMode    fillMode = FillMode::SOLID;
        CullMode    cullMode = CullMode::BACK;
        bool        frontCounterClockwise = false;
    };

    // Depth Stencil State
    struct DepthStencilState
    {
        bool            depthEnable = true;
        ComparisonFunc  depthFunc   = ComparisonFunc::LESS;
    };

    // Blend State for a single rendertarget
    struct RenderTargetBlendState
    {
        bool        blendEnable     = true;
        Blend       srcBlend        = Blend::ONE;
        Blend       destBlend       = Blend::ZERO;
        BlendOP     blendOp         = BlendOP::ADD;
        Blend       srcBlendAlpha   = Blend::ONE;
        Blend       destBlendAlpha  = Blend::ZERO;
        BlendOP     blendOpAlpha    = BlendOP::ADD;
        U8          writeMask       = 0x0f;   // Only least significant 4 bits used
    };

    // Complete blend state
    struct BlendState
    {
        bool                    alphaToCoverage = false;
        bool                    independentBlending = false;    // If false, only blendStates[0] will be used
        RenderTargetBlendState  blendStates[8];                 // Blend states for each render target
    };

}
