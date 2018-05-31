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
	float4 tangent : TANGENT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 WorldPos : POSITION;
	float3x3 TBN : TBN;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.WorldPos    = TO_WORLD_SPACE( vin.PosL );
    OUT.PosH 		= TO_CLIP_SPACE( vin.PosL );
	OUT.Tex 		= vin.tex;
	
	float3 tangent = vin.tangent.xyz;
	float3 T = TRANSFORM_NORMAL( tangent );
	float3 N = TRANSFORM_NORMAL( vin.normal ); 
	float3 B = cross( N, T ) * vin.tangent.w; 

	OUT.TBN = transpose( float3x3( T, B, N ) );
	
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
	float3 WorldPos : POSITION;
	float3x3 TBN : TBN;
};

Texture2D albedoMap : register(t3);
SamplerState samplerAlbedoMap : register(s3);

Texture2D roughnessMap : register(t4);
SamplerState samplerRoughnessMap : register(s4);  

Texture2D metallicMap : register(t5); 
SamplerState samplerMetallicMap : register(s5);  
 
Texture2D normalMap : register(t6); 
SamplerState samplerNormalMap : register(s6);  
 
 //----------------------------------------------------------------------
float getRoughness( float2 uv )
{
	return (1.0 - useRoughnessMap) * roughness + useRoughnessMap * roughnessMap.Sample( samplerRoughnessMap, uv ).r;
}
 
 //----------------------------------------------------------------------
float getMetallic( float2 uv )
{
	return (1.0 - useMetallicMap) * metallic + useMetallicMap * metallicMap.Sample( samplerMetallicMap, uv ).r;
}

//----------------------------------------------------------------------
float3 getNormal( float3x3 TBN, float2 uv )
{
	float3 normal = normalMap.Sample( samplerNormalMap, uv ).rgb; 
	normal = normalize( normal * 2.0 - 1.0 ); 
	return mul( TBN, normal ); 
} 
 
 //----------------------------------------------------------------------
float4 main(FragmentIn fin) : SV_Target
{
	float4 albedo = TO_LINEAR( albedoMap.Sample(samplerAlbedoMap, fin.Tex) );   
	float r = getRoughness( fin.Tex );   
	float m = getMetallic( fin.Tex );
	float3 normal = getNormal( fin.TBN, fin.Tex );

	float4 result = APPLY_LIGHTING( albedo * color, fin.WorldPos, normal, r, m );
	return result; 
}