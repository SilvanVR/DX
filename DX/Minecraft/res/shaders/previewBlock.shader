// ----------------------------------------------
#ZTest 			Off
#Queue 			Overlay

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float2 tex : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.WorldPos 	= TO_WORLD_SPACE( vin.PosL );
    OUT.PosH 		= TO_CLIP_SPACE( vin.PosL );
	OUT.Normal 		= TRANSFORM_NORMAL( vin.Normal );
	OUT.tex 		= vin.tex;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/shaders/includes/enginePS.hlsl"
#include "/shaders/lighting/blinn_phong.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

Texture2D tex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex.Sample(sampler0, fin.tex);
	
	// Don't light from below
	if (fin.Normal.y < 0)
		return textureColor * _Ambient;

	return APPLY_LIGHTING( textureColor, fin.WorldPos, fin.Normal );
}