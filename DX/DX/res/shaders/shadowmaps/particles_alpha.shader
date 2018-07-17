// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		On
#ZTest 			Less
#Queue 			Transparent
#AlphaToMask 	Off

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float2 tex : TEXCOORD;
	float4x4 modelToWorld : MODEL_INSTANCE;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;
	
	float4x4 modelToWorld = mul( _World, vin.modelToWorld );
	float4x4 mvp = mul( _ViewProj, modelToWorld );
	OUT.PosH = mul(mvp, float4(vin.PosL, 1));
	OUT.tex = vin.tex;
	
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

void main(FragmentIn fin)
{
	float4 textureColor = _MainTex.Sample(sampler0, fin.tex);
	if(textureColor.a < ALPHA_THRESHOLD)
		discard;
}