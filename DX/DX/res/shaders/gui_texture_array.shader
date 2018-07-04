// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#ZTest			Off
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