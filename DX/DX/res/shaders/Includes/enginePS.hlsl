// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.
// "light": Per light constant buffer.

#define MAX_LIGHTS 					16
#define MAX_SHADOWMAPS_2D			4
#define MAX_SHADOWMAPS_3D 			1
#define DIRECTIONAL_LIGHT 			0
#define POINT_LIGHT 				1
#define SPOT_LIGHT 					2
#define PI							3.14159265359
#define GAMMA						2.2
#define ALPHA_THRESHOLD  			0.1
#define SHADOW_TRANSITION_DISTANCE 	1
#define POISSON_DISK_SAMPLES 		16
#define MAX_CSM_SPLITS				4	
#define SHADOW_TYPE_NONE 			0
#define SHADOW_TYPE_HARD 			1
#define SHADOW_TYPE_SOFT			2
#define SHADOW_TYPE_CSM				3
#define SHADOW_TYPE_CSM_SOFT		4

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
	int			shadowType;				// 4 bytes	
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (4 * 16)
 
 
struct CSMSplit
{
	float4x4 vp;
	float range;
};

cbuffer cbBufferLights : register(b3)
{
	Light 		_Lights[MAX_LIGHTS]; // 16 * 80 = 1280 bytes (somehow 1024 bytes?!)
	float4x4 	_LightViewProj[MAX_SHADOWMAPS_2D]; // 4 * 64 = 256 bytes
	//float4x4 	_CSMLightViewProj[MAX_CSM_SPLITS]; // 4 * 64 = 256 bytes
	CSMSplit	_CSMSplits[MAX_CSM_SPLITS];
	int 		_LightCount; // 4 bytes
};

Texture2D shadowMap0 : register(t9);
SamplerState shadowMapSampler0 : register(s9);

Texture2D shadowMap1 : register(t10);
SamplerState shadowMapSampler1 : register(s10);

Texture2D shadowMap2 : register(t11);
SamplerState shadowMapSampler2 : register(s11);

Texture2D shadowMap3 : register(t12);
SamplerState shadowMapSampler3 : register(s12);

TextureCube<float4> shadowMapCube0 : register(t13);
SamplerState shadowMapCubeSampler0 : register(s13);

Texture2DArray shadowMapCascades : register(t14);
SamplerState shadowMapCascadesSampler : register(s14);

//**********************************************************************
// 2D SHADOWS (Dirlight + Spotlight)
//**********************************************************************

//-----------------------------------------------
float SAMPLE_SHADOWMAP_2D( int index, float2 uv )
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

static const float2 poissonDisk[POISSON_DISK_SAMPLES] = {
	 float2( -0.94201624, -0.39906216 ),  float2( 0.94558609, -0.76890725 ),
	 float2( -0.094184101, -0.92938870 ), float2( 0.34495938, 0.29387760 ),
	 float2( -0.91588581, 0.45771432 ),   float2( -0.81544232, -0.87912464 ),
	 float2( -0.38277543, 0.27676845 ),   float2( 0.97484398, 0.75648379 ),
	 float2( 0.44323325, -0.97511554 ),	  float2( 0.53742981, -0.47373420 ),
	 float2( -0.26496911, -0.41893023 ),  float2( 0.79197514, 0.19090188 ),
	 float2( -0.24188840, 0.99706507 ),	  float2( -0.81409955, 0.91437590 ),
	 float2( 0.19984126, 0.78641367 ),    float2( 0.14383161, -0.14100790 ) 
};

//-----------------------------------------------
// @Return: 1 if completely in light, otherwise <1 depending on how much samples are visible
//-----------------------------------------------
float PoisonDiskSampling( float currentDepth, float2 uv, int shadowMapIndex, float spacing )
{
	float invisibleSamples = 0;
	for (int i=0; i<POISSON_DISK_SAMPLES; ++i){
		float depthSample = SAMPLE_SHADOWMAP_2D( shadowMapIndex, uv + poissonDisk[i]/spacing );
		if ( currentDepth > depthSample )
			invisibleSamples++;
	}	
	return invisibleSamples / POISSON_DISK_SAMPLES;	
}

//-----------------------------------------------
// @Returns: 0 if in shadow (0-1 if on edge), 1 if in light
//-----------------------------------------------
float CALCULATE_SHADOW_DIR( float3 P, float shadowDistance, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float4 lightSpace = mul( _LightViewProj[shadowMapIndex], float4( P, 1 ) );
		float3 projCoords = lightSpace.xyz / lightSpace.w;
		float2 uv = projCoords.xy * 0.5 + 0.5;
		uv.y = 1 - uv.y;

		float currentDepth = projCoords.z;		
		float depthSample = SAMPLE_SHADOWMAP_2D( shadowMapIndex, uv );
				
		shadow = currentDepth > depthSample ? 1.0 : 0.0;	
				
		// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
		float distanceToCamera = length(P - _CameraPos);
		float transitionFactor = (shadowDistance - distanceToCamera) / SHADOW_TRANSITION_DISTANCE;
		transitionFactor = saturate( transitionFactor );

		shadow *= transitionFactor;	
	}	
	return 1-shadow;
}
float CALCULATE_SHADOW_DIR_SOFT( float3 P, float shadowDistance, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float4 lightSpace = mul( _LightViewProj[shadowMapIndex], float4( P, 1 ) );
		float3 projCoords = lightSpace.xyz / lightSpace.w;
		float2 uv = projCoords.xy * 0.5 + 0.5;
		uv.y = 1 - uv.y;

		float currentDepth = projCoords.z;
		
		// Poisson sampling soft shadows
		shadow = PoisonDiskSampling( currentDepth, uv, shadowMapIndex, 2048.0 );
						
		// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
		float distanceToCamera = length(P - _CameraPos);
		float transitionFactor = (shadowDistance - distanceToCamera) / SHADOW_TRANSITION_DISTANCE;
		transitionFactor = saturate(transitionFactor);

		shadow *= transitionFactor;	
	}	
	return 1-shadow;
}

//-----------------------------------------------
// @Returns: 0 if in shadow, 1 otherwise
//-----------------------------------------------
float CALCULATE_SHADOW_2D( float3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float4 lightSpace = mul( _LightViewProj[shadowMapIndex], float4( P, 1 ) );
		float3 projCoords = lightSpace.xyz / lightSpace.w;
		float2 uv = projCoords.xy * 0.5 + 0.5;
		uv.y = 1 - uv.y;

		float currentDepth = projCoords.z;
		float closestDepth = SAMPLE_SHADOWMAP_2D( shadowMapIndex, uv );
		shadow = currentDepth > closestDepth ? 1.0 : 0.0;
	}	
	return 1-shadow;
}
float CALCULATE_SHADOW_2D_SOFT( float3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float4 lightSpace = mul( _LightViewProj[shadowMapIndex], float4( P, 1 ) );
		float3 projCoords = lightSpace.xyz / lightSpace.w;
		float2 uv = projCoords.xy * 0.5 + 0.5;
		uv.y = 1 - uv.y;

		float currentDepth = projCoords.z;

		// Poisson sampling soft shadows
		shadow = PoisonDiskSampling( currentDepth, uv, shadowMapIndex, 512 );
	}	
	return 1-shadow;
}

//**********************************************************************
// 3D SHADOWS
//**********************************************************************

//-----------------------------------------------
float SAMPLE_SHADOWMAP_3D( int index, float3 uvw )
{
	float result = 0;
	switch(index)
	{
		case 0:	result = shadowMapCube0.SampleLevel( shadowMapCubeSampler0, uvw, 0 ).r; break;
	}
	return result;
}

//-----------------------------------------------
// @Returns: 0 if in shadow, 1 otherwise
//-----------------------------------------------
float CALCULATE_SHADOW_3D( float3 P, float3 L, float range, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float3 ld = P - L;
		float currentDepth = length( ld );	
		
		float closestDepth = SAMPLE_SHADOWMAP_3D( shadowMapIndex, ld );
		
		// Map back from [0,1] to world coordinates (range is equivalent to zfar)
		closestDepth *= range;
				
		// Give a higher bias the further away the depth sample is
		float bias = closestDepth * 0.1;
		shadow = currentDepth > closestDepth + bias ? 1.0 : 0.0;
	}	
	return 1-shadow;
}
float CALCULATE_SHADOW_3D_SOFT( float3 P, float3 L, float range, int shadowMapIndex )
{
	static const int NUM_SAMPLES = 20;
	float3 sampleOffsetDirections[20] = {
	   float3( 1,  1,  1), float3( 1, -1,  1), float3(-1, -1,  1), float3(-1,  1,  1), 
	   float3( 1,  1, -1), float3( 1, -1, -1), float3(-1, -1, -1), float3(-1,  1, -1),
	   float3( 1,  1,  0), float3( 1, -1,  0), float3(-1, -1,  0), float3(-1,  1,  0),
	   float3( 1,  0,  1), float3(-1,  0,  1), float3( 1,  0, -1), float3(-1,  0, -1),
	   float3( 0,  1,  1), float3( 0, -1,  1), float3( 0, -1, -1), float3( 0,  1, -1)
	}; 

	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float3 fragToLight = P - L;
		float currentDepth = length( fragToLight );	

		float diskRadius = 0.005;
		for(int i = 0; i < NUM_SAMPLES; ++i)
		{
			float closestDepth = SAMPLE_SHADOWMAP_3D( shadowMapIndex, fragToLight + sampleOffsetDirections[i] * diskRadius);
			closestDepth *= range; // Undo mapping [0;1]
			float bias = closestDepth * 0.1;
			if(currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		shadow /= float(NUM_SAMPLES); 
	}
	return 1-shadow;
}

//**********************************************************************
// CASCADED SHADOW MAPPING
//**********************************************************************

//-----------------------------------------------
// @Returns: 0 if in shadow (0-1 if on edge), 1 if in light
//-----------------------------------------------
float CALCULATE_SHADOW_CSM( float3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float distanceToCamera = length(P - _CameraPos);
		
		int width, height, elements;
		shadowMapCascades.GetDimensions( width, height, elements );
		for (int cascade = 0; cascade < elements; ++cascade)
		{
			if (distanceToCamera < _CSMSplits[cascade].range)
			{
				float4 lightSpace = mul( _CSMSplits[cascade].vp, float4( P, 1 ) );
				float3 projCoords = lightSpace.xyz / lightSpace.w;
				float2 uv = projCoords.xy * 0.5 + 0.5;
				uv.y = 1 - uv.y;
					
				float3 uvw = float3( uv, cascade );
				float depthSample = shadowMapCascades.SampleLevel( shadowMapCascadesSampler, uvw, 0 ).r;
				
				float currentDepth = projCoords.z;				
				shadow = currentDepth > depthSample ? 1.0 : 0.0;
				
				// Smoothly transition shadow to nothing in last cascade
				if (cascade == (elements-1))
				{
					// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
					const float transitionDistance = 30.0f;
					float transitionFactor = (_CSMSplits[cascade].range - distanceToCamera) / transitionDistance;
					transitionFactor = saturate( transitionFactor );
					shadow *= transitionFactor;	
				}			
				break;
			}		
		}		
	}	
	return 1-shadow;
}

//-----------------------------------------------
float CALCULATE_SHADOW_CSM_SOFT( float3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float distanceToCamera = length(P - _CameraPos);
		
		int width, height, elements;
		shadowMapCascades.GetDimensions( width, height, elements );
		for (int cascade = 0; cascade < elements; ++cascade)
		{
			if (distanceToCamera < _CSMSplits[cascade].range)
			{
				float4 lightSpace = mul( _CSMSplits[cascade].vp, float4( P, 1 ) );
				float3 projCoords = lightSpace.xyz / lightSpace.w;
				float2 uv = projCoords.xy * 0.5 + 0.5;
				uv.y = 1 - uv.y;
				
				// Poisson sampling soft shadows
				float currentDepth = projCoords.z;
				
				float invisibleSamples = 0;
				for (int i=0; i<POISSON_DISK_SAMPLES; ++i){
					float2 uvMod = uv + poissonDisk[i]/2048.0;
					float3 uvw = float3( uvMod, cascade );
					float depthSample = shadowMapCascades.SampleLevel( shadowMapCascadesSampler, uvw, 0 ).r;
					
					if ( currentDepth > depthSample )
						invisibleSamples++;
				}	
				shadow = invisibleSamples / POISSON_DISK_SAMPLES;
				
				// Smoothly transition shadow to nothing in last cascade
				if (cascade == (elements-1))
				{
					// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
					const float transitionDistance = 30.0f;
					float transitionFactor = (_CSMSplits[cascade].range - distanceToCamera) / transitionDistance;
					transitionFactor = saturate( transitionFactor );
					shadow *= transitionFactor;	
				}			
				break;
			}		
		}		
	}	
	return 1-shadow;
}

//-----------------------------------------------
float4 VISUALIZE_CASCADES( float3 P )
{
	float4 colors[] = {
		float4(1,0,0,1),
		float4(0,1,0,1),
		float4(0,0,1,1),
		float4(1,1,0,1)
	};

	float distanceToCamera = length(P - _CameraPos);
	
	int width, height, elements;
	shadowMapCascades.GetDimensions( width, height, elements );	
	for (int cascade = 0; cascade < elements; cascade++)
	{
		if (distanceToCamera < _CSMSplits[cascade].range)
			return colors[cascade];
	}	
	return float4(1,1,1,1);
}

//**********************************************************************
// MISC FUNCTIONS
//**********************************************************************

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