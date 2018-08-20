// ----------------------------------------------
#Fill			Wireframe
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
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE(vin.PosL);

    return OUT;
}

// ----------------------------------------------
#shader fragment

struct FragmentIn
{
    float4 PosH : SV_POSITION;
};

float4 main(FragmentIn fin) : SV_Target
{
    return float4(1,1,1,1);
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

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(1,1,1,1);
}
