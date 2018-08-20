// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.
// "light": Per light constant buffer.

cbuffer cbPerCamera : register(b0)
{	
	float4x4 _View;	
	float4x4 _Proj;
    float4x4 _ViewProj;
	float3 _CameraPos;
	float _zNear;
	float _zFar;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 _World;
};

cbuffer cbBufferGlobal : register(b2)
{
	float _Time;
	float _Ambient;
};

//----------------------------------------------------------------------
float4 TO_CLIP_SPACE( float4 vert )
{
	float4x4 mvp = mul( _ViewProj, _World );
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
    return mul( _World, float4( vert, 1 ) ).xyz;
}

//----------------------------------------------------------------------
float3 TRANSFORM_NORMAL( float3 normal )
{
	return normalize( mul( (float3x3)_World, normal ) );
}
