// ----------------------------------------------
#Fill			Solid
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			Transparent

// ----------------------------------------------
#shader vertex

#include "includes/engine.inc"

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float4 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE(vin.PosL);
	OUT.WorldPos = mul(gWorld, float4(vin.PosL, 1.0f));
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float4 WorldPos : POSITION;
};

cbuffer cbPerMaterial
{
	float opacity;
};

Texture2D tex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex.Sample(sampler0, fin.WorldPos.xz);
	return float4(textureColor.rgb, opacity);
}