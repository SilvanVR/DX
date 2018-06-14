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

cbuffer cbBufferGlobal : register(b2)
{	
	float _Time;
	float _Ambient;
};
