cbuffer cbPerCamera
{
    float4x4 gViewProj;
};

cbuffer cbPerObject
{
    float4x4 gWorld;
};

float4 TO_CLIP_SPACE(float3 vert)
{
	float4x4 mvp = mul(gViewProj, gWorld);
    return mul(mvp, float4(vert,1));
}

float4 TO_CLIP_SPACE(float4 vert)
{
	float4x4 mvp = mul(gViewProj, gWorld);
    return mul(mvp, vert);
}