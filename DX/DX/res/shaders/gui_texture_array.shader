// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#ZTest			Off
#Blend 			SrcAlpha OneMinusSrcAlpha
#Scissor		On
#Queue			Overlay

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
	float4 color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH 	= mul( _Proj, float4( vin.PosL,1 ) );
	OUT.tex 	= vin.tex;
	OUT.color 	= vin.color;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

cbuffer cbPerMaterial
{
	int slice;
};

Texture2DArray _MainTex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = _MainTex.Sample( sampler0, float3(fin.tex, slice) );
	return textureColor * fin.color;
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UV;
layout (location = 2) in vec4 VERTEX_COLOR;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

void main()
{
	outUV = VERTEX_UV;
	outColor = VERTEX_COLOR;
	gl_Position = VULKAN_CLIP * _Camera.proj * vec4( VERTEX_POSITION, 1 );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform sampler2DArray _MainTex;
layout (set = SET_FIRST, binding = 1) uniform Material
{
	int slice;
} material;

void main()
{
	vec4 textureColor = texture( _MainTex, vec3(inUV, material.slice) );
	outColor = textureColor * inColor;
}