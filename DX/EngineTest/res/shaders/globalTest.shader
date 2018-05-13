// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Priority 		Transparent

// ----------------------------------------------
#shader vertex

#include "includes/engineVS.hlsl"

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

	//float si = (sin(gTime) + 1) * 0.5;
	//float3 newPos = vin.PosL + float3(0,0,sin(gTime));
    OUT.PosH = TO_CLIP_SPACE(vin.PosL);
	OUT.tex = vin.tex;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "includes/enginePS.hlsl"

cbuffer cbPerMaterial
{
	float test;
	float4 tintColor;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D tex0;
SamplerState sampler0;


float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex0.Sample(sampler0, fin.tex);
	
	float si = (sin(gTime) + 1) * 0.5;
	return textureColor * si;
}