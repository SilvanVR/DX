// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#Blend 			SrcAlpha OneMinusSrcAlpha
#Scissor		On
#Queue			Overlay

// ----------------------------------------------
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

Texture2D tex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex.Sample( sampler0, fin.tex );
	return textureColor * fin.color;
}