// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float3 normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH   = TO_CLIP_SPACE( vin.PosL );
	OUT.Normal = TRANSFORM_NORMAL( vin.normal ); 					
	
    return OUT;
}

// ----------------------------------------------
#shader fragment    

#include "/engine/shaders/includes/enginePS.hlsl" 

struct FragmentIn  
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

//----------------------------------------------------------------------
float4 main(FragmentIn fin) : SV_Target
{
	return float4(fin.Normal,1); 
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec3 VERTEX_NORMAL;

layout (location = 0) out vec3 outNormal;

void main()
{
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
	outNormal = TRANSFORM_NORMAL( VERTEX_NORMAL );
}

// ----------------------------------------------
#shader fragment    

#include "/engine/shaders/includes/vulkan/engineFS.glsl" 

layout (location = 0) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

//----------------------------------------------------------------------
void main()
{
	outColor = vec4( inNormal, 1 );
}