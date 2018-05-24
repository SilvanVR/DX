// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			Geometry
#AlphaToMask 	Off

// ----------------------------------------------
#shader vertex

#include "includes/engineVS.hlsl"

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

    OUT.PosH = TO_CLIP_SPACE(vin.PosL);
    OUT.Color = vin.Color;

    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(FragmentIn fin) : SV_Target
{
    return fin.Color;
}