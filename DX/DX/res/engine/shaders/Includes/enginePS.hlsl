// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.
// "light": Per light constant buffer.

#define MAX_LIGHTS 			16
#define MAX_SHADOWMAPS		4
#define DIRECTIONAL_LIGHT 	0
#define POINT_LIGHT 		1
#define SPOT_LIGHT 			2
#define PI					3.14159265359
#define GAMMA				2.2

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

struct Light
{
    float3      position;               // 16 bytes
	int         lightType;              // 4 bytes
    //----------------------------------- (16 byte boundary)
    float3      direction;              // 12 bytes
	float 		intensity;				// 4 bytes
    //----------------------------------- (16 byte boundary)
    float4      color;                  // 16 bytes
    //----------------------------------- (16 byte boundary)
    float       spotAngle;              // 4 bytes
    float       range;					// 4 bytes
	float2		PADDING;				// 8 bytes	
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (4 * 16)
 
cbuffer cbBufferLights : register(b3)
{
	Light 		_Lights[MAX_LIGHTS];
	int 		_LightCount;
	float4x4 	_LightViewProj[MAX_SHADOWMAPS];
};

Texture2D shadowMap : register(t8);
SamplerState shadowMapSampler : register(s8);

// ----------------------------------------------
float4 TO_LINEAR( float4 color )
{
	return float4( pow( abs( color.rgb ), GAMMA ), color.a );
}
float3 TO_LINEAR( float3 color )
{
	return pow( abs( color ), GAMMA );
}

float3 TO_SRGB( float3 color )
{
	return pow( abs( color ), 1.0/GAMMA ); 
}