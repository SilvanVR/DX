// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.
// "light": Per light constant buffer.

cbuffer cbPerCamera : register(b0)
{
    float4x4 gViewProj;	
	float3 gCameraPos;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
};

cbuffer cbBufferGlobal : register(b2)
{
	float gTime;
	float gAmbient;
};

//----------------------------------------------------------------------
float4 TO_CLIP_SPACE( float4 vert )
{
	float4x4 mvp = mul( gViewProj, gWorld );
    return mul( mvp, vert );
}

//----------------------------------------------------------------------
float4 TO_CLIP_SPACE( float3 vert )
{
    return TO_CLIP_SPACE( float4( vert, 1 ) );
}

//----------------------------------------------------------------------
float3 TO_WORLD_SPACE( float3 vert )
{
    return mul( gWorld, float4( vert, 1 ) ).xyz;
}


//----------------------------------------------------------------------
float3 TRANSFORM_NORMAL( float3 normal )
{
	return normalize( mul( gWorld, float4( normal, 0.0f ) ).xyz );
}
