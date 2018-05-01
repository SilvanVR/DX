#pragma once
/**********************************************************************
    class: None (enums.hpp)

    author: S. Hau
    date: March 7, 2018

    Contains definitions for important enums within a graphics
    application.
**********************************************************************/


namespace Graphics {

    enum class ShaderType
    {
        Unknown = -1,
        Vertex,
        Fragment,
        Geometry,
        Tessellation,
        NUM_SHADER_TYPES
    };

    // Available index-formats for an index buffer
    enum class IndexFormat
    {
        U16 = 1,
        U32
    };

    // Determines buffer types / update mechanism
    enum class BufferUsage
    {
        Immutable = 1,  // Can't be updated once created.
        LongLived,      // Expected to be updated occasionally.
        Frequently      // Expected to be updated every frame.
    };

    // Topology for a submesh
    enum class MeshTopology
    {
        Triangles = 1,
        TriangleStrip,
        Lines,
        LineStrip,
        Points
    };

    // Rasterization fill mode
    enum class FillMode
    {
        Solid = 1,
        Wireframe
    };

    // Rasterization cull mode
    enum class CullMode
    {
        None = 1,
        Front,
        Back
    };

    // Comparison Functions
    enum class ComparisonFunc
    {
        Never = 1,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    // Blend values for the blend-state
    enum class Blend
    {
        Zero = 1,
        One,
        SrcColor,
        InvSrcColor,
        SrcAlpha,
        InvSrcAlpha,
        DestAlpha,
        InvDestAlpha,
        DestColor,
        InvDestColor,
        SrcAlphaSat,
        BlendFactor,
        InvBlendFactor,
        Src1Color,
        InvSrc1Color,
        Src1Alpha,
        InvSrc1Alpha
    };

    // Blend operations for the blending state
    enum class BlendOP
    {
       Add = 1,
       Subtract,
       RevSubtract,
       Min,
       Max
    };

    //----------------------------------------------------------------------
    enum class TextureDimension
    {
        Unknown = 0,
        Tex2D,
        Tex3D,
        Cube,
        Tex2DArray,
        CubeArray
    };

    //----------------------------------------------------------------------
    enum class TextureFormat
    {
        Unknown = 0,
        Alpha8,             // Alpha - only texture format.
        ARGB4444,           // A 16 bits / pixel texture format. Texture stores color with an alpha channel.
        RGBA32,             // Color with alpha texture format, 8 - bits per channel.
        RGB565,             // A 16 bit color texture format.
        R16,                // A 16 bit color texture format that only has a red channel.
        BGRA32,             // Color with alpha texture format, 8 - bits per channel.
        RHalf,              // Scalar(R) texture format, 16 bit floating point.
        RGHalf,             // Two color(RG) texture format, 16 bit floating point per channel.
        RGBAHalf,           // RGB color and alpha texture format, 16 bit floating point per channel.
        RFloat,             // Scalar(R) texture format, 32 bit floating point.
        RGFloat,            // Two color(RG) texture format, 32 bit floating point per channel.
        RGBAFloat,          // RGB color and alpha texture format, 32 - bit floats per channel.
        YUY2,               // A format that uses the YUV color space and is often used for video encoding or playback.
        RGB9e5Float,        // RGB HDR format, with 9 bit mantissa per channel and a 5 bit shared exponent.
        BC4,                // Compressed one channel(R) texture format.
        BC5,                // Compressed two - channel(RG) texture format.
        BC6H,               // HDR compressed color texture format.
        BC7,                // High quality compressed color texture format.
        RG16,               // Two color(RG) texture format, 8 - bits per channel.
        R8,                 // Scalar(R) render texture format, 8 bit fixed point.
    };

    //----------------------------------------------------------------------
    enum class TextureFilter
    {
        Point, 
        Bilinear,
        Trilinear
    };

    //----------------------------------------------------------------------
    enum class TextureAddressMode
    {
        Repeat,
        Clamp,
        Mirror,
        MirrorOnce
    };
}
