
cbuffer cbPerMaterial
{
	int texIndex;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2DArray texArray;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = texArray.Sample(sampler0, float3(fin.tex,texIndex));	
	return textureColor;
}