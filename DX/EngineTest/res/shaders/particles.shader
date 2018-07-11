// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Offw
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

    OUT.PosH = TO_CLIP_SPACE(vin.PosL + float3(vin.tex * 100,0));
	OUT.tex = 1-vin.tex;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D _MainTex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = _MainTex.Sample(sampler0, fin.tex);	
	return textureColor;
}