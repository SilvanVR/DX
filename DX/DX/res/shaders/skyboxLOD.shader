// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZTest			Off
#Queue 			Background

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 tex : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;
	
	OUT.tex = vin.PosL;
	
    float4 clipPos = TO_CLIP_SPACE( float4(vin.PosL, 0.0f));
	
	// Little trick which places the object very close to the far-plane
	OUT.PosH = clipPos.xyww;
	OUT.PosH.z *= 0.999999;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

cbuffer cbPerMaterial
{ 
	float lod;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 tex : POSITION;
};

TextureCube<float4> Cubemap;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = Cubemap.SampleLevel(sampler0, normalize(fin.tex), lod);
	
	return textureColor;
}