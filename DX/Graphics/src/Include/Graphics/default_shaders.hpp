#pragma once
/**********************************************************************
    class: None (default_shaders.hpp)

    author: S. Hau
    date: March 24, 2018

    Contains the default shaders in an ASCII text format.
**********************************************************************/

#define API_D3D11

namespace Graphics { namespace ShaderSources {

#ifdef API_D3D11

    const String ERROR_VERTEX =                    "\
    cbuffer cbPerCamera : register( b0 )            \
    {                                               \
        float4x4 gViewProj;                         \
        float3 gCameraPos;                          \
    };                                              \
                                                    \
    cbuffer cbPerObject : register( b1 )            \
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

    const String ERROR_FRAGMENT =                  "\
    struct FragmentIn                               \
    {                                               \
        float4 PosH : SV_POSITION;                  \
    };                                              \
                                                    \
    float4 main(FragmentIn fin) : SV_Target         \
    {                                               \
        return float4(1,0,1,1);                     \
    }";

    const String DEFAULT_VERTEX = ERROR_VERTEX;

    const String DEFAULT_FRAGMENT =                "\
    struct FragmentIn                               \
    {                                               \
        float4 PosH : SV_POSITION;                  \
    };                                              \
                                                    \
    float4 main(FragmentIn fin) : SV_Target         \
    {                                               \
        return float4(1,1,1,1);                     \
    }";

    const String COLOR_VERTEX =                    "\
    cbuffer cbPerCamera : register( b0 )            \
    {                                               \
        float4x4 gViewProj;                         \
	    float3 gCameraPos;                          \
    };                                              \
                                                    \
    cbuffer cbPerObject : register( b1 )            \
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

    const String COLOR_FRAGMENT =                  "\
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


    const String POST_PROCESS_VERTEX =                 "\
    struct VertexOut                                    \
    {                                                   \
        float4 PosH : SV_POSITION;                      \
        float2 UV : TEXCOORD0;                          \
    };                                                  \
                                                        \
    VertexOut main(uint vI : SV_VERTEXID)               \
    {                                                   \
        VertexOut OUT;                                  \
                                                        \
        float2 uv = float2((vI << 1) & 2, vI & 2);      \
        OUT.UV = float2(uv.x, 1-uv.y);                  \
        OUT.PosH = float4(uv * 2.0f - 1.0f, 0.0f, 1.0f);\
                                                        \
        return OUT;                                     \
    }";

    const String POST_PROCESS_FRAGMENT =               "\
    struct FragmentIn                                   \
    {                                                   \
        float4 PosH : SV_POSITION;                      \
        float2 UV : TEXCOORD0;                          \
    };                                                  \
                                                        \
    Texture2D _MainTex;                                 \
    SamplerState _Sampler0;                             \
                                                        \
    float4 main(FragmentIn fin) : SV_Target             \
    {                                                   \
        float4 color = _MainTex.Sample(_Sampler0, fin.UV);\
        return color;                                   \
    }";

#elif
    static_assert("Default shader for every graphics api must be set here as a string.")
#endif


} } // End namespaces