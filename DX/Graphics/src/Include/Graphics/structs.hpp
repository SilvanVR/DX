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

    // GPU Information
    struct GPUDescription
    {
        String name;
        Size maxDedicatedMemoryMB;
    };

    // Collected render information (per camera)
    struct FrameInfo
    {
        U32 drawCalls;
        U32 numVertices;
        U32 numTriangles;
        U32 numLights;
    };

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
        FillMode    fillMode                = FillMode::Solid;
        CullMode    cullMode                = CullMode::Back;
        I32         depthBias               = 0;
        F32         depthBiasClamp          = 0.0f;
        F32         slopeScaledDepthBias    = 0.0f;
        bool        frontCounterClockwise   = false;
        bool        scissorEnable           = false;
        bool        depthClipEnable         = true;
    };

    // Depth Stencil State
    struct DepthStencilState
    {
        bool            depthEnable = true;
        bool            depthWrite  = true;
        ComparisonFunc  depthFunc   = ComparisonFunc::Less;
    };

    // Blend State for a single rendertarget
    struct RenderTargetBlendState
    {
        bool        blendEnable     = true;
        Blend       srcBlend        = Blend::One;
        Blend       destBlend       = Blend::Zero;
        BlendOP     blendOp         = BlendOP::Add;
        Blend       srcBlendAlpha   = Blend::One;
        Blend       destBlendAlpha  = Blend::Zero;
        BlendOP     blendOpAlpha    = BlendOP::Add;
        U8          writeMask       = 0x0f;   // Only least significant 4 bits used
    };

    // Complete blend state
    struct BlendState
    {
        bool                    alphaToCoverage = false;
        bool                    independentBlending = false;    // If false, only blendStates[0] will be used
        RenderTargetBlendState  blendStates[8];                 // Blend states for each render target
    };

    // Multisampling description
    struct SamplingDescription
    {
        U32 count   = 1;
        U32 quality = 0;
    };

}
