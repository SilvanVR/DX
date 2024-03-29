// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZTest			Off
#Queue 			Background

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
    float3 tex : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;
	
	OUT.tex = vin.PosL;
	
    float4 clipPos = TO_CLIP_SPACE( float4(vin.PosL, 0.0f));
	
	// Little trick which places the object very close to the far-plane
	OUT.PosH = clipPos.xyww;
	OUT.PosH.z *= 0.999999;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 tex : POSITION;
};

TextureCube<float4> Cubemap;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = Cubemap.Sample(sampler0, normalize(fin.tex));	
	return textureColor;
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;

layout (location = 0) out vec3 outUVW;

void main()
{
    outUVW = VERTEX_POSITION;
	
	vec4 clipPos = TO_CLIP_SPACE( vec4(VERTEX_POSITION, 0.0f) );
	
	// Little trick which places the object very close to the far-plane
	clipPos = clipPos.xyww;
	clipPos.z *= 0.999999;
	gl_Position = clipPos;
}
 
// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

// In data
layout (location = 0) in vec3 inUVW;

// Out data
layout (location = 0) out vec4 outColor; 

// Descriptor-Sets
layout (set = SET_FIRST, binding = 0) uniform samplerCube Cubemap;
 
void main()
{
	outColor = texture(Cubemap, normalize(inUVW));
}