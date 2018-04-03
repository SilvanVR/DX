
cbuffer cbPerMaterial
{
	float4 tintColor;
};

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
	
	return textureColor * tintColor;
}