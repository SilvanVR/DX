
cbuffer cbPerCamera
{
    float4x4 gViewProj;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
};

struct VertexIn
{
    float3 PosL : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

Texture2D tex;
SamplerState sampler0;

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

	float4 height = tex.SampleLevel(sampler0, vin.PosL.xy, 0);
	float4 pos = float4(vin.PosL, 1.0f) + height;
	
    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, pos);
	OUT.Color = vin.Color; 
	
    return OUT;
}