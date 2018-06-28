// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

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

#include "/engine/shaders/includes/enginePS.hlsl"
#include "/engine/shaders/includes/blinn_phong.hlsl"

cbuffer cbPerMaterial
{
	float uvScale;
}

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPos : POSITION;
};

Texture2D _MainTex;
SamplerState sampler0;

static const float ALPHA_THRESHOLD = 0.1f;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = _MainTex.Sample(sampler0, fin.Tex * uvScale);
	//float4 depth = shadowMap.Sample(shadowMapSampler, fin.Tex);
	
	//return float4(fin.Normal,1);
	if (textureColor.a < ALPHA_THRESHOLD)
		discard;
		
	float4 lightSpace = mul( _LightViewProj[0], float4(fin.WorldPos, 1) );
	float3 projCoords = lightSpace.xyz / lightSpace.w;
	float2 uv = projCoords.xy * 0.5 + 0.5;
	uv.y = 1 - uv.y;
	
	float currentDepth = projCoords.z;
	float closestDepth = shadowMap.Sample( shadowMapSampler, uv ).r;
	float shadow = currentDepth < closestDepth ? 0.0 : 1.0;

	if ( !inRange(currentDepth) || !inRange(uv.x) || !inRange(uv.y))
		shadow = 0;
		
	return APPLY_LIGHTING( textureColor, fin.WorldPos, fin.Normal );// * (1-shadow); 
}