// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry
#DepthBias		0
#DBSlopeScaled  0
#DBClamp 		0

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE( vin.PosL );
	OUT.WorldPos = TO_WORLD_SPACE( vin.PosL );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment  

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 WorldPos : POSITION;
};

float main(FragmentIn fin) : SV_Depth
{
	float lightDistance = length(_CameraPos - fin.WorldPos);
	// Map to [0,1] range
	lightDistance /= _zFar;
	return lightDistance;
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 inPos;

layout (location = 0) out vec3 outWorldPos;

void main()
{
	outWorldPos = TO_WORLD_SPACE( inPos );
	gl_Position = TO_CLIP_SPACE( inPos );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec3 inWorldPos;

void main()
{
	float lightDistance = length(_Camera.pos - inWorldPos);
	// Map to [0,1] range
	lightDistance /= _Camera.zFar;
	gl_FragDepth = lightDistance;
}