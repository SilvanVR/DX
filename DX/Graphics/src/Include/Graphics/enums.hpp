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
        Unknown,
        Vertex,
        Fragment,
        Geometry,
        Tessellation
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
        Quads,
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
        Alpha8, //	Alpha - only texture format.
        ARGB4444, //	A 16 bits / pixel texture format.Texture stores color with an alpha channel.
        RGB24, //	Color texture format, 8 - bits per channel.
        RGBA32, //	Color with alpha texture format, 8 - bits per channel.
        ARGB32, //	Color with alpha texture format, 8 - bits per channel.
        RGB565, //	A 16 bit color texture format.
        R16, //	A 16 bit color texture format that only has a red channel.
        DXT1, //	Compressed color texture format.
        DXT5, //	Compressed color with alpha channel texture format.
        RGBA4444, //	Color and alpha texture format, 4 bit per channel.
        BGRA32, //	Color with alpha texture format, 8 - bits per channel.
        RHalf, //	Scalar(R) texture format, 16 bit floating point.
        RGHalf, //	Two color(RG) texture format, 16 bit floating point per channel.
        RGBAHalf, //	RGB color and alpha texture format, 16 bit floating point per channel.
        RFloat, //	Scalar(R) texture format, 32 bit floating point.
        RGFloat, //	Two color(RG) texture format, 32 bit floating point per channel.
        RGBAFloat, //	RGB color and alpha texture format, 32 - bit floats per channel.
        YUY2, //	A format that uses the YUV color space and is often used for video encoding or playback.
        RGB9e5Float, //	RGB HDR format, with 9 bit mantissa per channel and a 5 bit shared exponent.
        BC4, //	Compressed one channel(R) texture format.
        BC5, //	Compressed two - channel(RG) texture format.
        BC6H, //	HDR compressed color texture format.
        BC7, //	High quality compressed color texture format.
        DXT1Crunched, //	Compressed color texture format with Crunch compression for smaller storage sizes.
        DXT5Crunched, //	Compressed color with alpha channel texture format with Crunch compression for smaller storage sizes.
        ETC_RGB4, //	ETC(GLES2.0) 4 bits / pixel compressed RGB texture format.
        ATC_RGB4, //	ATC(ATITC) 4 bits / pixel compressed RGB texture format.
        ATC_RGBA8, //	ATC(ATITC) 8 bits / pixel compressed RGB texture format.
        EAC_R, //	ETC2 / EAC(GL ES 3.0) 4 bits / pixel compressed unsigned single - channel texture format.
        EAC_R_SIGNED, //	ETC2 / EAC(GL ES 3.0) 4 bits / pixel compressed signed single - channel texture format.
        EAC_RG, //	ETC2 / EAC(GL ES 3.0) 8 bits / pixel compressed unsigned dual - channel(RG) texture format.
        EAC_RG_SIGNED, //	ETC2 / EAC(GL ES 3.0) 8 bits / pixel compressed signed dual - channel(RG) texture format.
        ETC2_RGB, //	ETC2(GL ES 3.0) 4 bits / pixel compressed RGB texture format.
        ETC2_RGBA1, //	ETC2(GL ES 3.0) 4 bits / pixel RGB + 1 - bit alpha texture format.
        ETC2_RGBA8, //	ETC2(GL ES 3.0) 8 bits / pixel compressed RGBA texture format.
        ASTC_RGB_4x4, //	ASTC(4x4 pixel block in 128 bits) compressed RGB texture format.
        ASTC_RGB_5x5, //	ASTC(5x5 pixel block in 128 bits) compressed RGB texture format.
        ASTC_RGB_6x6, //	ASTC(6x6 pixel block in 128 bits) compressed RGB texture format.
        ASTC_RGB_8x8, //	ASTC(8x8 pixel block in 128 bits) compressed RGB texture format.
        ASTC_RGB_10x10, //	ASTC(10x10 pixel block in 128 bits) compressed RGB texture format.
        ASTC_RGB_12x12, //	ASTC(12x12 pixel block in 128 bits) compressed RGB texture format.
        ASTC_RGBA_4x4, //	ASTC(4x4 pixel block in 128 bits) compressed RGBA texture format.
        ASTC_RGBA_5x5, //	ASTC(5x5 pixel block in 128 bits) compressed RGBA texture format.
        ASTC_RGBA_6x6, //	ASTC(6x6 pixel block in 128 bits) compressed RGBA texture format.
        ASTC_RGBA_8x8, //	ASTC(8x8 pixel block in 128 bits) compressed RGBA texture format.
        ASTC_RGBA_10x10, //	ASTC(10x10 pixel block in 128 bits) compressed RGBA texture format.
        ASTC_RGBA_12x12, //	ASTC(12x12 pixel block in 128 bits) compressed RGBA texture format.
        ETC_RGB4_3DS, //	ETC 4 bits / pixel compressed RGB texture format.
        ETC_RGBA8_3DS, //	ETC 4 bits / pixel RGB + 4 bits / pixel Alpha compressed texture format.
        RG16, //	Two color(RG) texture format, 8 - bits per channel.
        R8, //	Scalar(R) render texture format, 8 bit fixed point.
        ETC_RGB4Crunched, //	Compressed color texture format with Crunch compression for smaller storage sizes.
        ETC2_RGBA8Crunched, //Compressed color with alpha channel texture format with Crunch compression for smaller storage sizes.
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
