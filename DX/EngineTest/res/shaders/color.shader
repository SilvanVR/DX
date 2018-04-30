// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Priority 		1000
#AlphaToMask 	Off

// ----------------------------------------------
#shader vertex

cbuffer cbPerCamera
{
    float4x4 gViewProj;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));
    OUT.Color = vin.Color;

    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "test/test.shader"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(FragmentIn fin) : SV_Target
{
    return fin.Color;
}