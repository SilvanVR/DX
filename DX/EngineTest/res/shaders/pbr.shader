// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			Geometry

// ----------------------------------------------
#shader vertex

#include "includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	//float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    //float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.WorldPos 	= TO_WORLD_SPACE( vin.PosL );
    OUT.PosH 		= TO_CLIP_SPACE( vin.PosL );
	//OUT.Tex 		= vin.tex;
	OUT.Normal 		= TRANSFORM_NORMAL( vin.normal );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment    

#include "includes/enginePS.hlsl"
#include "includes/pbr.hlsl"

cbuffer cbPerMaterial
{ 
	float4 color;
	float metallic;
	float roughness;
	float ao;
};

struct FragmentIn 
{
    float4 PosH : SV_POSITION;
	//float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

Texture2D albedo;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	//float4 textureColor = albedo.Sample(sampler0, fin.Tex);
	float4 result = APPLY_LIGHTING( color, fin.WorldPos, fin.Normal, roughness, metallic );
	return result; 
}