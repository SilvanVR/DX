
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
SamplerState sampler0;

Texture2D dirt;
SamplerState sampler1;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = shaderTexture.Sample(sampler0, fin.tex);
	float4 textureColor2 = dirt.Sample(sampler1, fin.tex);
	return textureColor * textureColor2 * tintColor;
}