// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Priority 		Geometry
#Blend 			SrcAlpha OneMinusSrcAlpha

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL 		: POSITION;
	float2 tex 			: TEXCOORD0;
	float3 normal 		: NORMAL;
	int4   boneIDs 		: BONEID;
	float4 boneWeights 	: BONEWEIGHT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
};

cbuffer cbShader
{
	float4x4 u_boneTransforms[MAX_BONES];
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.Tex 		= vin.tex;
	OUT.Normal 		= TRANSFORM_NORMAL( vin.normal );

	float4x4 boneTransform = u_boneTransforms[vin.boneIDs[0]] * vin.boneWeights[0];
	boneTransform += u_boneTransforms[vin.boneIDs[1]] * vin.boneWeights[1];
	boneTransform += u_boneTransforms[vin.boneIDs[2]] * vin.boneWeights[2];
	boneTransform += u_boneTransforms[vin.boneIDs[3]] * vin.boneWeights[3]; 
	
	float4 pos = mul( boneTransform, float4( vin.PosL, 1 ) );
    OUT.PosH = TO_CLIP_SPACE( pos );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"
#include "/engine/shaders/includes/blinn_phong.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Normal : NORMAL;
};
 
Texture2D tex;
SamplerState sampler0; 
        
float4 main(FragmentIn fin) : SV_Target
{            
	float4 textureColor = tex.Sample(sampler0, fin.Tex);		                              
	return textureColor;
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

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outWorldPos;

void main()
{
	outUV 		= VERTEX_UV;
	outNormal 	= TRANSFORM_NORMAL( VERTEX_NORMAL );
	outWorldPos = TO_WORLD_SPACE( VERTEX_POSITION );
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
}
 
// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inWorldPos;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform sampler2D tex;
 
void main()
{
	vec4 textureColor = texture( tex, inUV );
 
	outColor = textureColor;
}