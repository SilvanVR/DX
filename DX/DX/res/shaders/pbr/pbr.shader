// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry
#AlphaToMask 	On

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

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

	OUT.TBN = float3x3( T.x, B.x, N.x,
	                    T.y, B.y, N.y, 
						T.z, B.z, N.z );						
	
    return OUT;
}

// ----------------------------------------------
#shader fragment    

#include "/engine/shaders/includes/enginePS.hlsl" 
#include "/engine/shaders/includes/pbr.hlsl"

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

	if (albedo.a < ALPHA_THRESHOLD)
		discard; 
	
	float4 result = APPLY_LIGHTING( albedo * color, fin.WorldPos, normal, r, m );
	return result; 
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UV;
layout (location = 2) in vec3 VERTEX_NORMAL;
layout (location = 3) in vec4 VERTEX_TANGENT;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outWorldPos;
layout (location = 2) out mat3 outTBN;

void main()
{
	outWorldPos = TO_WORLD_SPACE( VERTEX_POSITION );
    gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
	outUV 		= VERTEX_UV;
	
	vec3 tangent = VERTEX_TANGENT.xyz;
	vec3 T = TRANSFORM_NORMAL( tangent );
	vec3 N = TRANSFORM_NORMAL( VERTEX_NORMAL ); 
	vec3 B = cross( N, T ) * VERTEX_TANGENT.w;

	outTBN = mat3( T, B, N );
}

// ----------------------------------------------
#shader fragment    

#include "/engine/shaders/includes/vulkan/engineFS.glsl" 
#include "/engine/shaders/includes/vulkan/pbr.glsl"

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inWorldPos;
layout (location = 2) in mat3 inTBN;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform MATERIAL
{
	vec4 color;
	float metallic;
	float roughness;
	float useRoughnessMap;
	float useMetallicMap;
};
layout (set = SET_FIRST, binding = 1) uniform sampler2D albedoMap;
layout (set = SET_FIRST, binding = 2) uniform sampler2D roughnessMap;
layout (set = SET_FIRST, binding = 3) uniform sampler2D metallicMap;
layout (set = SET_FIRST, binding = 4) uniform sampler2D normalMap;

 
//----------------------------------------------------------------------
float getRoughness( vec2 uv )
{
	return (1.0 - useRoughnessMap) * roughness + useRoughnessMap * texture( roughnessMap, uv ).r;
}
 
//----------------------------------------------------------------------
float getMetallic( vec2 uv )
{ 
	return (1.0 - useMetallicMap) * metallic + useMetallicMap * texture( metallicMap, uv ).r;
} 

//----------------------------------------------------------------------
vec3 getNormal( mat3 TBN, vec2 uv )
{
	vec3 normal = texture( normalMap, uv ).rgb; 
	normal = normalize( normal * 2.0 - 1.0 ); 
	return TBN * normal;  
}
 
//----------------------------------------------------------------------
void main()
{
	vec4 albedo = TO_LINEAR( texture( albedoMap, inUV ) );   
	float r = getRoughness( inUV );   
	float m = getMetallic( inUV );
	vec3 normal = getNormal( inTBN, inUV );

	if (albedo.a < ALPHA_THRESHOLD)
		discard; 
	
	outColor = APPLY_LIGHTING( albedo * color, inWorldPos, normal, r, m );
}