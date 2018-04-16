
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
	float2 Material : TEXCOORD0;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float  MaterialID : MATERIAL;
	float4 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));
	OUT.Color = vin.Color;
	OUT.Normal = normalize(vin.Normal);
	OUT.MaterialID = vin.Material.x;
	OUT.WorldPos = mul(gWorld, float4(vin.PosL, 1.0f));
	
    return OUT;
}