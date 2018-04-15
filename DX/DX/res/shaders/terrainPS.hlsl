
cbuffer cbPerMaterial
{
	float4 color;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
};

float4 main(FragmentIn fin) : SV_Target
{
	return float4(fin.Normal, 1);
}