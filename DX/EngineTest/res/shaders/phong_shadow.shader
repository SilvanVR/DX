// ----------------------------------------------
#Fill			Solid
#Cull 			None
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
        
float4 main(FragmentIn fin) : SV_Target
{            
	float4 textureColor = _MainTex.Sample(sampler0, fin.Tex * uvScale);
	   
	if (textureColor.a < ALPHA_THRESHOLD) 
		discard;        
		
	//return VISUALIZE_CASCADES(fin.WorldPos);                                 
	return APPLY_LIGHTING( textureColor, fin.WorldPos, fin.Normal ); 
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
#include "/engine/shaders/includes/vulkan/blinn_phong.glsl"

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inWorldPos;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform sampler2D _MainTex;
layout (set = SET_FIRST, binding = 1) uniform Material
{
	float uvScale;
} material;
 
void main()
{
	vec4 textureColor = texture( _MainTex, inUV * material.uvScale );
	
	if (textureColor.a < ALPHA_THRESHOLD) 
		discard;  
	
	outColor = APPLY_LIGHTING( textureColor, inWorldPos, inNormal );
}