// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry
#DepthBias		3
#DBSlopeScaled  4
#DBClamp 		0
#DepthClip		Off

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
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE( vin.PosL );
	OUT.Tex = vin.tex;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment  

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

Texture2D _MainTex;
SamplerState sampler0;

void main(FragmentIn fin)
{
	float4 textureColor = _MainTex.Sample(sampler0, fin.Tex);
	if (textureColor.a < ALPHA_THRESHOLD)
		discard;
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UV;

layout (location = 0) out vec2 outUV;

void main()
{
	outUV = VERTEX_UV;
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec2 inUV;

// Descriptor-Sets
layout (set = SET_FIRST, binding = 0) uniform sampler2D Albedo_S;

void main()
{
	vec4 textureColor = texture( Albedo_S, inUV );
	if (textureColor.a < ALPHA_THRESHOLD)
		discard;
}