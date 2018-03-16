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
        U16 = 1,
        U32
    };

    // Determines buffer types / update mechanism
    enum class BufferUsage
    {
        IMMUTABLE = 1,  // Can't be updated once created.
        LONG_LIVED,     // Expected to be updated occasionally.
        FREQUENTLY      // Expected to be updated every frame.
    };

    // Topology for a submesh
    enum class MeshTopology
    {
        Triangles = 1,
        Quads,
        Lines,
        LineStrip,
        Points
    };

    // Rasterization fill mode
    enum class FillMode
    {
        SOLID = 1,
        WIREFRAME
    };

    // Rasterization cull mode
    enum class CullMode
    {
        NONE = 1,
        FRONT,
        BACK
    };

    // Comparison Functions
    enum class ComparisonFunc
    {
        NEVER = 1,
        LESS,
        EQUAL,
        LESS_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_EQUAL,
        ALWAYS
    };

    // Blend values for the blend-state
    enum class Blend
    {
        ZERO = 1,
        ONE,
        SRC_COLOR,
        INV_SRC_COLOR,
        SRC_ALPHA,
        INV_SRC_ALPHA,
        DEST_ALPHA,
        INV_DEST_ALPHA,
        DEST_COLOR,
        INV_DEST_COLOR,
        SRC_ALPHA_SAT,
        BLEND_FACTOR,
        INV_BLEND_FACTOR,
        SRC1_COLOR,
        INV_SRC1_COLOR,
        SRC1_ALPHA,
        INV_SRC1_ALPHA
    };

    // Blend operations for the blending state
    enum class BlendOP
    {
       ADD = 1,
       SUBTRACT,
       REV_SUBTRACT,
       MIN,
       MAX
    };
}
