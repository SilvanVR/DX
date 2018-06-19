// ----------------------------------------------
#Fill			Solid
#Cull			Back
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			Transparent

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float3 Normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH 		= TO_CLIP_SPACE( float4(vin.PosL,1) );
	OUT.WorldPos 	= TO_WORLD_SPACE( vin.PosL );
	OUT.Normal 		= TRANSFORM_NORMAL( vin.Normal );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"
#include "/shaders/lighting/blinn_phong.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 WorldPos : POSITION;
	float3 Normal : NORMAL;
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
	return textureColor;
	float4 result = APPLY_LIGHTING(textureColor, fin.WorldPos, fin.Normal);
	
	return float4(result.rgb, opacity);
}