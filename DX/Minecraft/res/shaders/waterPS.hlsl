struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float4 WorldPos : POSITION;
};

cbuffer cbPerMaterial
{
	float opacity;
};

Texture2D tex;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = tex.Sample(sampler0, fin.WorldPos.xz);
	return float4(textureColor.rgb, opacity);
}