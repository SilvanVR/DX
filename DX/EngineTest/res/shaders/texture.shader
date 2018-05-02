// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Priority 		1000
#AlphaToMask 	Off

// ----------------------------------------------
#shader vertex

#include "includes/engine.inc"

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

cbuffer cbPerMaterial
{
	float4 tintColor;
	float mix;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D tex0;
SamplerState sampler0;

Texture2D tex1;
SamplerState sampler1;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex0.Sample(sampler0, fin.tex);
	float4 textureColor2 = tex1.Sample(sampler1, fin.tex);
	
	float4 combined = lerp(textureColor, textureColor2, mix);
	return combined * tintColor;
}