
cbuffer cbPerCamera
{
    float4x4 gViewProj;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
};

cbuffer cbPerMaterial
{
    float4 mColor;
	float val;
};

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    float4x4 mvp = mul(gViewProj, gWorld);
    OUT.PosH = mul(mvp, float4(vin.PosL, 1.0f));
	OUT.Color = mColor;
	
    return OUT;
}