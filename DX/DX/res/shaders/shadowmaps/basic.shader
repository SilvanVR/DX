// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry
#DepthBias		50
#DBSlopeScaled  3
#DBClamp 		0
#DepthClip		Off

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

#include "/engine/shaders/includes/enginePS.hlsl"

void main(float4 PosH : SV_POSITION)
{
	// Shader writes only depth
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 inPos;

void main()
{
	gl_Position = TO_CLIP_SPACE( inPos );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

void main()
{
	// Shader writes only depth
}