// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float3 Normal : NORMAL;
	float2 Material : TEXCOORD0;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Material : MATERIAL;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;
	
	OUT.WorldPos 	= TO_WORLD_SPACE( vin.PosL );
    OUT.PosH 		= TO_CLIP_SPACE( vin.PosL );
	OUT.Normal 		= TRANSFORM_NORMAL( vin.Normal );
	OUT.Material 	= vin.Material;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/shaders/includes/enginePS.hlsl"
#include "/shaders/lighting/blinn_phong.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Material : MATERIAL; // X Coord contains side-block, Y Top/Bottom block
	float3 WorldPos : POSITION;
};

Texture2DArray texArray;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{	
	// Compute UV-Coordinates by projecting the world-pos along the respective axis
	float3 uvw;
	if (fin.Normal.x > 0.9 || fin.Normal.x < -0.9)
		uvw = float3( -fin.WorldPos.zy + 0.5, fin.Material.x );
	else if (fin.Normal.y > 0.9 || fin.Normal.y < -0.9)
		uvw = float3( fin.WorldPos.xz + 0.5,fin.Material.y );
	else 
		uvw = float3( -fin.WorldPos.xy + 0.5, fin.Material.x );
	
	float4 textureColor = texArray.Sample(sampler0, uvw);
	
	// Don't light from below blocks
	if (fin.Normal.y < 0)
		return textureColor * _Ambient;
	
	return APPLY_LIGHTING( textureColor, fin.WorldPos, fin.Normal );
}