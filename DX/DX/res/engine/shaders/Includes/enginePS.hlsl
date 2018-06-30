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
#define ALPHA_THRESHOLD  	0.1f

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
    int       	shadowMapIndex;			// 4 bytes
	float		PADDING;				// 4 bytes	
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (4 * 16)
 
cbuffer cbBufferLights : register(b3)
{
	Light 		_Lights[MAX_LIGHTS];
	int 		_LightCount;
	float4x4 	_LightViewProj[MAX_SHADOWMAPS];
};

Texture2D shadowMap0 : register(t9);
SamplerState shadowMapSampler0 : register(s9);

Texture2D shadowMap1 : register(t10);
SamplerState shadowMapSampler1 : register(s10);

Texture2D shadowMap2 : register(t11);
SamplerState shadowMapSampler2 : register(s11);

Texture2D shadowMap3 : register(t12);
SamplerState shadowMapSampler3 : register(s12);

//-----------------------------------------------
float SAMPLE_SHADOWMAP( int index, float2 uv )
{
	float result = 0;
	switch(index)
	{
		case 0:	result = shadowMap0.Sample( shadowMapSampler0, uv ).r; break;
		case 1: result = shadowMap1.Sample( shadowMapSampler1, uv ).r; break;
		case 2: result = shadowMap2.Sample( shadowMapSampler2, uv ).r; break;
		case 3: result = shadowMap3.Sample( shadowMapSampler3, uv ).r; break;
	}
	return result;
}

//----------------------------------------------------------------------
bool inRange( float val )
{
	return val >= 0.001 && val < 0.999;
}

//-----------------------------------------------
float CALCULATE_SHADOW( float3 P,  int shadowMapIndex )
{
	float shadow = 1.0f;		
	if (shadowMapIndex >= 0)
	{
		float4 lightSpace = mul( _LightViewProj[shadowMapIndex], float4( P, 1 ) );
		float3 projCoords = lightSpace.xyz / lightSpace.w;
		float2 uv = projCoords.xy * 0.5 + 0.5;
		uv.y = 1 - uv.y;

		float currentDepth = projCoords.z;
		if ( inRange(currentDepth) && inRange(uv.x) && inRange(uv.y) )
		{	
			float closestDepth = SAMPLE_SHADOWMAP( shadowMapIndex, uv );
			shadow = currentDepth < closestDepth ? 1.0 : 0.0;
		}
	}	
	return shadow;
}

//-----------------------------------------------
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