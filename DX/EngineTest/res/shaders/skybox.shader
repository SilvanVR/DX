// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#Priority 		10000

// ----------------------------------------------
#shader vertex

#include "includes/engine.inc"

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 tex : TEXCOORD0;
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

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 tex : TEXCOORD0;
};

TextureCube<float4> Cubemap;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = Cubemap.Sample(sampler0, normalize(fin.tex));
	
	return textureColor;
}