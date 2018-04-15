
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
	float3 Normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));
	OUT.Color = vin.Color;
	OUT.Normal = vin.Normal;
	
    return OUT;
}