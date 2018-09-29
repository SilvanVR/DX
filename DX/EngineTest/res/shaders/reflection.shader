// ----------------------------------------------
#Fill			Solid
#Cull 			None
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
    float3 Normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 tex : POSITION;
    float3 normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;
	
	OUT.tex = vin.PosL;
	OUT.PosH = TO_CLIP_SPACE( vin.PosL );
	OUT.normal = vin.Normal;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 tex : POSITION;
	float3 normal : NORMAL;
};

TextureCube<float4> Cubemap;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float3 refl = reflect(normalize(-fin.tex), normalize(fin.normal));
	return Cubemap.Sample(sampler0, refl);
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec3 VERTEX_NORMAL;

layout (location = 0) out vec3 outUVW;
layout (location = 1) out vec3 outNormal;

void main()
{
    outUVW = VERTEX_POSITION;
	outNormal = VERTEX_NORMAL;
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
}
 
// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

// In data
layout (location = 0) in vec3 inUVW;
layout (location = 1) in vec3 inNormal;

// Out data
layout (location = 0) out vec4 outColor; 

// Descriptor-Sets
layout (set = SET_FIRST, binding = 0) uniform samplerCube Cubemap;
 
void main()
{
	outColor = texture(Cubemap, reflect(normalize(-inUVW), normalize(inNormal)));
}