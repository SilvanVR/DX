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

float4 main( float3 PosL : POSITION ) : SV_POSITION
{
	return TO_CLIP_SPACE( PosL );
}

// ----------------------------------------------
#shader fragment

float4 main( float4 PosH : SV_POSITION ) : SV_Target
{
	return float4(1,0,1,1);
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;

void main()
{
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/version.glsl"

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(1,0,1,1);
}
