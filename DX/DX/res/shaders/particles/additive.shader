// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		Off
#ZTest 			Less
#Blend 			Add Add
#Queue 			Transparent
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
	float2 tex : TEXCOORD;
	float4 color : COLOR_INSTANCE;
	float4x4 modelToWorld : MODEL_INSTANCE;
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
	
	float4x4 modelToWorld = mul( _World, vin.modelToWorld );
	float4x4 mvp = mul( _ViewProj, modelToWorld );
	OUT.PosH = mul(mvp, float4(vin.PosL, 1));
	OUT.tex = vin.tex;
	OUT.color = vin.color;
	
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

Texture2D _MainTex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = _MainTex.Sample(sampler0, fin.tex);
	float4 finalColor = textureColor * fin.color;
	if (finalColor.a < ALPHA_THRESHOLD)
		discard;
	return finalColor;
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UV;
layout (location = 2) in vec4 COLOR_INSTANCE;
layout (location = 3) in mat4 MODEL_INSTANCE;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

void main()
{
	mat4 mvp = VULKAN_CLIP * _Camera.proj * _Camera.view * _Object.world * MODEL_INSTANCE;
	
	outUV = VERTEX_UV;
	outColor = COLOR_INSTANCE;
	gl_Position = mvp * vec4( VERTEX_POSITION, 1 );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

// In data
layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;

// Out data
layout (location = 0) out vec4 outColor;

// Descriptor-Sets
layout (set = SET_FIRST, binding = 0) uniform sampler2D _MainTex;

void main()
{
	outColor = texture(_MainTex, inUV) * inColor;
}