#pragma once
/**********************************************************************
    class: None (default_shader.hpp)

    author: S. Hau
    date: March 24, 2018

    Contains the default shaders in an ASCII text format.
**********************************************************************/

#define D3D11

namespace Core { namespace Resources {

#ifdef D3D11

    const String ERROR_VERTEX_SHADER_SOURCE = "     \
    cbuffer cbPerCamera                             \
    {                                               \
        float4x4 gViewProj;                         \
    };                                              \
                                                    \
    cbuffer cbPerObject                             \
    {                                               \
        float4x4 gWorld;                            \
    };                                              \
                                                    \
    struct VertexIn                                 \
    {                                               \
        float3 PosL : POSITION;                     \
    };                                              \
                                                    \
    struct VertexOut                                \
    {                                               \
        float4 PosH : SV_POSITION;                  \
    };                                              \
                                                    \
    VertexOut main(VertexIn vin)                    \
    {                                               \
        VertexOut OUT;                              \
                                                    \
        float4x4 mvp = mul(gViewProj, gWorld);      \
        OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));\
                                                    \
        return OUT;                                 \
    }";

    const String ERROR_FRAGMENT_SHADER_SOURCE = "   \
    struct FragmentIn                               \
    {                                               \
        float4 PosH : SV_POSITION;                  \
    };                                              \
                                                    \
    float4 main(FragmentIn fin) : SV_Target         \
    {                                               \
        return float4(1,0,1,1);                     \
    }";

    const String DEFAULT_VERTEX_SHADER_SOURCE =  "  \
    cbuffer cbPerCamera                             \
    {                                               \
        float4x4 gViewProj;                         \
    };                                              \
                                                    \
    cbuffer cbPerObject                             \
    {                                               \
        float4x4 gWorld;                            \
    };                                              \
                                                    \
    struct VertexIn                                 \
    {                                               \
        float3 PosL : POSITION;                     \
        float4 Color : COLOR;                       \
    };                                              \
                                                    \
    struct VertexOut                                \
    {                                               \
        float4 PosH : SV_POSITION;                  \
        float4 Color : COLOR;                       \
    };                                              \
                                                    \
    VertexOut main(VertexIn vin)                    \
    {                                               \
        VertexOut OUT;                              \
                                                    \
        float4x4 mvp = mul(gViewProj, gWorld);      \
        OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));\
        OUT.Color = vin.Color;                      \
                                                    \
        return OUT;                                 \
    }";

    const String DEFAULT_FRAGMENT_SHADER_SOURCE = " \
    struct FragmentIn                               \
    {                                               \
        float4 PosH : SV_POSITION;                  \
        float4 Color : COLOR;                       \
    };                                              \
                                                    \
    float4 main(FragmentIn fin) : SV_Target         \
    {                                               \
        return fin.Color;                           \
    }";

#elif
    static_assert("Default shader for every graphics api must be set here as a string.")
#endif



} }