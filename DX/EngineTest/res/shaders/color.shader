// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

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

#include "/engine/shaders/includes/enginePS.hlsl"

cbuffer cbPerMaterial
{
	float4 color;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(FragmentIn fin) : SV_Target
{
    return fin.Color * color;
}