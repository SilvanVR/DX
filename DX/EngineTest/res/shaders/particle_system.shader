// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Priority 		Transparent
#AlphaToMask 	Off

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float2 tex : TEXCOORD_INSTANCED;
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
	float4 tintColor;
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
	return textureColor * tintColor;
}