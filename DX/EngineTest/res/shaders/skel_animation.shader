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
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	OUT.Tex = vin.tex;	

	float4x4 boneTransform = _BoneTransforms[vin.boneIDs[0]] * vin.boneWeights[0];
	boneTransform += _BoneTransforms[vin.boneIDs[1]] * vin.boneWeights[1];
	boneTransform += _BoneTransforms[vin.boneIDs[2]] * vin.boneWeights[2];
	boneTransform += _BoneTransforms[vin.boneIDs[3]] * vin.boneWeights[3]; 
	
	float4 pos = mul( boneTransform, float4( vin.PosL, 1 ) );
    OUT.PosH = TO_CLIP_SPACE( pos );
	
	float3 normal = mul( boneTransform, float4( vin.normal, 0 ) ).xyz;
	OUT.Normal = TRANSFORM_NORMAL( normal );
	
	OUT.WorldPos = TO_WORLD_SPACE( pos.xyz );
	
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
	float3 WorldPos : POSITION;
};
 
Texture2D tex;
SamplerState sampler0; 
        
float4 main(FragmentIn fin) : SV_Target
{            
	float4 textureColor = tex.Sample(sampler0, fin.Tex);		                              
	return APPLY_LIGHTING( textureColor, fin.WorldPos, fin.Normal ); 
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3  VERTEX_POSITION;
layout (location = 1) in vec2  VERTEX_UV;
layout (location = 2) in vec3  VERTEX_NORMAL;
layout (location = 3) in ivec4 VERTEX_BONEID;
layout (location = 4) in vec4  VERTEX_BONEWEIGHT;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outWorldPos;

void main()
{
	outUV = VERTEX_UV;

	mat4 boneTransform = _Animation.boneTransforms[VERTEX_BONEID[0]] * VERTEX_BONEWEIGHT[0];
	boneTransform += _Animation.boneTransforms[VERTEX_BONEID[1]] * VERTEX_BONEWEIGHT[1];
	boneTransform += _Animation.boneTransforms[VERTEX_BONEID[2]] * VERTEX_BONEWEIGHT[2];
	boneTransform += _Animation.boneTransforms[VERTEX_BONEID[3]] * VERTEX_BONEWEIGHT[3]; 
	
	vec3 normal = ( boneTransform * vec4( VERTEX_NORMAL, 0 ) ).xyz;
	outNormal = TRANSFORM_NORMAL( normal );
	
	vec4 pos = boneTransform * vec4( VERTEX_POSITION, 1 );	
	outWorldPos = TO_WORLD_SPACE( pos.xyz );
	
	gl_Position = TO_CLIP_SPACE( pos );
}
 
// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"
#include "/engine/shaders/includes/vulkan/blinn_phong.glsl"

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inWorldPos;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST + 1, binding = 0) uniform sampler2D tex;
 
void main()
{
	vec4 textureColor = texture( tex, inUV );
	outColor = APPLY_LIGHTING( textureColor, inWorldPos, inNormal );
}