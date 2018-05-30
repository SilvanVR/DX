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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.WorldPos 	= TO_WORLD_SPACE( vin.PosL );
    OUT.PosH 		= TO_CLIP_SPACE( vin.PosL );
	OUT.Tex 		= vin.tex;
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
	float useRoughnessMap;
	float useMetallicMap;
};

struct FragmentIn  
{
    float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL; 
	float3 WorldPos : POSITION;
};

Texture2D albedoMap : register(t3);
SamplerState samplerAlbedoMap : register(s3);

Texture2D roughnessMap : register(t4);
SamplerState samplerRoughnessMap : register(s4);  

Texture2D metallicMap : register(t5);
SamplerState samplerMetallicMap : register(s5);  
 
float getRoughness( float2 uv )
{
	return (1.0 - useRoughnessMap) * roughness + useRoughnessMap * roughnessMap.Sample( samplerRoughnessMap, uv ).r;
}

float getMetallic( float2 uv )
{
	return (1.0 - useMetallicMap) * metallic + useMetallicMap * metallicMap.Sample( samplerMetallicMap, uv ).r;
}
 
float4 main(FragmentIn fin) : SV_Target
{
	float4 albedo = TO_LINEAR( albedoMap.Sample(samplerAlbedoMap, fin.Tex) );
	float r = getRoughness( fin.Tex );
	float m = getMetallic( fin.Tex );
	
	float4 result = APPLY_LIGHTING( albedo * color, fin.WorldPos, fin.Normal, r, m );
	return result; 
}