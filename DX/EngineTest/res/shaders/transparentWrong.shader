// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			0
#AlphaToMask 	Off

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
    float2 tex : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE(vin.PosL);
	OUT.tex = vin.tex;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

cbuffer cbPerMaterial
{
	float opacity;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D tex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex.Sample(sampler0, fin.tex);
	return float4(textureColor.rgb, opacity);
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UV;

layout (location = 1) out vec2 outUV;

void main()
{
	outUV = VERTEX_UV;
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

// Descriptor-Sets
layout (set = SET_FIRST, binding = 0) uniform sampler2D tex;
layout (set = SET_FIRST, binding = 1) uniform UBOPerMaterial
{
	float opacity;
} ubo;

void main()
{
	outColor = texture(tex, inUV) * ubo.opacity;
}
