
cbuffer cbPerMaterial
{
	float4 tintColor;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = shaderTexture.Sample(SampleType, fin.tex);
	return textureColor * tintColor;
}