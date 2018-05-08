// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer. Should contain things like time.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.

cbuffer cbPerCamera : register(b0)
{
    float4x4 gViewProj;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
};

cbuffer cbBufferGlobal : register(b2)
{
	float4 gColor;
    float gTime;
};

//----------------------------------------------------------------------
float4 TO_CLIP_SPACE(float4 vert)
{
	float4x4 mvp = mul( gViewProj, gWorld );
    return mul( mvp, vert );
}

//----------------------------------------------------------------------
float4 TO_CLIP_SPACE(float3 vert)
{
    return TO_CLIP_SPACE( float4( vert, 1 ) );
}
