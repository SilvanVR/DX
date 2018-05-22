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
	//float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    //float2 tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE(vin.PosL);
	//OUT.tex = vin.tex;
	OUT.Normal = vin.normal;
	OUT.WorldPos = mul(gWorld, float4(vin.PosL, 1.0f));
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "includes/enginePS.hlsl"

cbuffer cbPerMaterial
{
	float test;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	//float2 tex : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 WorldPos : POSITION;
};

//Texture2D tex0;
//SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	//float4 textureColor = tex0.Sample(sampler0, fin.tex);
	
	float4 normal = float4(fin.Normal, 1);
	
	//return normal;
	return APPLY_LIGHTING(fin.WorldPos.xyz, fin.Normal);  
	
	//return textureColor;
}