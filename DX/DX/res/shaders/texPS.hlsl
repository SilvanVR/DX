
cbuffer cbPerMaterial
{
	float4 tintColor;
	float mix;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D tex0;
SamplerState sampler0;

Texture2D tex1;
SamplerState sampler1;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex0.Sample(sampler0, fin.tex);
	float4 textureColor2 = tex1.Sample(sampler1, fin.tex);
	
	float4 combined = lerp(textureColor, textureColor2, 0.5);
	return combined * tintColor;
}