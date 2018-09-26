#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_LIGHTS 					16
#define ALPHA_THRESHOLD 			0.1
#define SET_FIRST                   1

#define MAX_SHADOWMAPS_2D			4
#define MAX_SHADOWMAPS_3D 			1
#define DIRECTIONAL_LIGHT 			0
#define POINT_LIGHT 				1
#define SPOT_LIGHT 					2
#define PI							3.14159265359
#define GAMMA						2.2
#define SHADOW_TRANSITION_DISTANCE 	1
#define POISSON_DISK_SAMPLES 		16
#define MAX_CSM_SPLITS				4	
#define SHADOW_TYPE_NONE 			0
#define SHADOW_TYPE_HARD 			1
#define SHADOW_TYPE_SOFT			2
#define SHADOW_TYPE_CSM				3
#define SHADOW_TYPE_CSM_SOFT		4

// Vulkan clip space has inverted Y. This fixes it.
const mat4 VULKAN_CLIP = { { 1.0,  0.0, 0.0, 0.0 },
                           { 0.0, -1.0, 0.0, 0.0 },
                           { 0.0,  0.0, 1.0, 0.0 },
                           { 0.0,  0.0, 0.0, 1.0 } };

//----------------------------------------------------------------------
// Descriptor-Sets
//----------------------------------------------------------------------

// Per frame global ubo
layout (set = 0, binding = 0) uniform GLOBAL
{
	float _Time;
	float _Ambient;
} _Global;

// Per camera ubo
layout (set = 0, binding = 1) uniform CAMERA
{
	mat4 view;
	mat4 proj;	
	vec3 pos;
	float PAD;
	float zNear;
	float zFar;
} _Camera;

struct Light
{
    vec3      	position;               // 12 bytes
	int         lightType;              // 4 bytes
    //----------------------------------- (16 byte boundary)
    vec3      	direction;              // 12 bytes
	float 		intensity;				// 4 bytes
    //----------------------------------- (16 byte boundary)
    vec4        color;                  // 16 bytes
    //----------------------------------- (16 byte boundary)
    float       spotAngle;              // 4 bytes
    float       range;					// 4 bytes
    int       	shadowMapIndex;			// 4 bytes
	int			shadowType;				// 4 bytes	
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (4 * 16)
 
struct CSMSplit
{
	mat4 vp;
	float range;
};
 
// Light ubo
layout (set = 0, binding = 2) uniform LIGHTS
{
	Light 	 lights[MAX_LIGHTS];
	mat4 	 viewProj[MAX_SHADOWMAPS_2D];
	CSMSplit CSMSplits[MAX_CSM_SPLITS];
	int 	 count; // 4 bytes
} _Lights;

layout (set = 0, binding = 3) uniform sampler2D shadowMap0;
layout (set = 0, binding = 4) uniform sampler2D shadowMap1;
layout (set = 0, binding = 5) uniform sampler2D shadowMap2;
layout (set = 0, binding = 6) uniform sampler2D shadowMap3;
layout (set = 0, binding = 7) uniform samplerCube shadowMapCube0;
layout (set = 0, binding = 8) uniform sampler2DArray shadowMapCascades;

//-----------------------------------------------
float saturate( float val )
{
	return clamp( val, 0, 1 );
}

//**********************************************************************
// 2D SHADOWS (Dirlight + Spotlight)
//**********************************************************************

//-----------------------------------------------
float SAMPLE_SHADOWMAP_2D( int index, vec2 uv )
{
	float result = 0;
	switch(index)
	{
		case 0:	result = texture( shadowMap0, uv ).r; break;
		case 1: result = texture( shadowMap1, uv ).r; break;
		case 2: result = texture( shadowMap2, uv ).r; break;
		case 3: result = texture( shadowMap3, uv ).r; break;
	}
	return result;
}

//----------------------------------------------------------------------
bool inRange( float val )
{
	return val >= 0.001 && val < 0.999;
}

const vec2 poissonDisk[POISSON_DISK_SAMPLES] = {
	 vec2( -0.94201624, -0.39906216 ),  vec2( 0.94558609, -0.76890725 ),
	 vec2( -0.094184101, -0.92938870 ), vec2( 0.34495938, 0.29387760 ),
	 vec2( -0.91588581, 0.45771432 ),   vec2( -0.81544232, -0.87912464 ),
	 vec2( -0.38277543, 0.27676845 ),   vec2( 0.97484398, 0.75648379 ),
	 vec2( 0.44323325, -0.97511554 ),	  vec2( 0.53742981, -0.47373420 ),
	 vec2( -0.26496911, -0.41893023 ),  vec2( 0.79197514, 0.19090188 ),
	 vec2( -0.24188840, 0.99706507 ),	  vec2( -0.81409955, 0.91437590 ),
	 vec2( 0.19984126, 0.78641367 ),    vec2( 0.14383161, -0.14100790 ) 
};

//-----------------------------------------------
// @Return: 1 if completely in light, otherwise <1 depending on how much samples are visible
//-----------------------------------------------
float PoisonDiskSampling( float currentDepth, vec2 uv, int shadowMapIndex, float spacing )
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
float CALCULATE_SHADOW_DIR( vec3 P, float shadowDistance, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		vec4 lightSpace = VULKAN_CLIP * _Lights.viewProj[shadowMapIndex] * vec4( P, 1 );
		vec3 projCoords = lightSpace.xyz / lightSpace.w;
		vec2 uv = projCoords.xy * 0.5 + 0.5;

		float currentDepth = projCoords.z;		
		float depthSample = SAMPLE_SHADOWMAP_2D( shadowMapIndex, uv );
				
		shadow = currentDepth > depthSample ? 1.0 : 0.0;	
				
		// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
		float distanceToCamera = length(P - _Camera.pos);
		float transitionFactor = (shadowDistance - distanceToCamera) / SHADOW_TRANSITION_DISTANCE;
		transitionFactor = saturate( transitionFactor );

		shadow *= transitionFactor;	
	}	
	return 1-shadow;
}
float CALCULATE_SHADOW_DIR_SOFT( vec3 P, float shadowDistance, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		vec4 lightSpace = VULKAN_CLIP * _Lights.viewProj[shadowMapIndex] * vec4( P, 1 );
		vec3 projCoords = lightSpace.xyz / lightSpace.w;
		vec2 uv = projCoords.xy * 0.5 + 0.5;

		float currentDepth = projCoords.z;
		
		// Poisson sampling soft shadows
		shadow = PoisonDiskSampling( currentDepth, uv, shadowMapIndex, 2048.0 );
						
		// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
		float distanceToCamera = length(P - _Camera.pos);
		float transitionFactor = (shadowDistance - distanceToCamera) / SHADOW_TRANSITION_DISTANCE;
		transitionFactor = saturate(transitionFactor);

		shadow *= transitionFactor;	
	}	
	return 1-shadow;
}

//-----------------------------------------------
// @Returns: 0 if in shadow, 1 otherwise
//-----------------------------------------------
float CALCULATE_SHADOW_2D( vec3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		vec4 lightSpace = VULKAN_CLIP * _Lights.viewProj[shadowMapIndex] * vec4( P, 1 );
		vec3 projCoords = lightSpace.xyz / lightSpace.w;
		vec2 uv = projCoords.xy * 0.5 + 0.5;

		float currentDepth = projCoords.z;
		float depthSample = SAMPLE_SHADOWMAP_2D( shadowMapIndex, uv );
		shadow = currentDepth > depthSample ? 1.0 : 0.0;
	}	
	return 1-shadow;
}
float CALCULATE_SHADOW_2D_SOFT( vec3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		vec4 lightSpace = VULKAN_CLIP * _Lights.viewProj[shadowMapIndex] * vec4( P, 1 );
		vec3 projCoords = lightSpace.xyz / lightSpace.w;
		vec2 uv = projCoords.xy * 0.5 + 0.5;

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
float SAMPLE_SHADOWMAP_3D( int index, vec3 uvw )
{
	float result = 0;
	switch(index)
	{
		case 0:	result = texture( shadowMapCube0, normalize(uvw) ).r; break;
	}
	return result;
}

//-----------------------------------------------
// @Returns: 0 if in shadow, 1 otherwise
//-----------------------------------------------
float CALCULATE_SHADOW_3D( vec3 P, vec3 L, float range, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		vec3 ld = P - L;
		float currentDepth = length( ld );	
		
		float closestDepth = SAMPLE_SHADOWMAP_3D( shadowMapIndex, ld );
		
		// Map back from [0,1] to world coordinates (range is equivalent to zfar)
		closestDepth *= range;
				
		// Give a higher bias the further away the depth sample is
		float bias = closestDepth * 0.1;
		shadow = currentDepth > (closestDepth + bias) ? 1.0 : 0.0;
	}	
	return 1-shadow;
}
float CALCULATE_SHADOW_3D_SOFT( vec3 P, vec3 L, float range, int shadowMapIndex )
{
	const int NUM_SAMPLES = 20;
	vec3 sampleOffsetDirections[20] = {
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	}; 

	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		vec3 fragToLight = P - L;
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
float CALCULATE_SHADOW_CSM( vec3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float distanceToCamera = length(P - _Camera.pos);
		
		ivec3 texSize = textureSize( shadowMapCascades, 0 );
		float elements = texSize[2];
		for (int cascade = 0; cascade < elements; ++cascade)
		{
			if (distanceToCamera < _Lights.CSMSplits[cascade].range)
			{
				vec4 lightSpace = VULKAN_CLIP * _Lights.CSMSplits[cascade].vp * vec4( P, 1 );
				vec3 projCoords = lightSpace.xyz / lightSpace.w;
				vec2 uv = projCoords.xy * 0.5 + 0.5;
					
				vec3 uvw = vec3( uv, cascade );
				float depthSample = texture( shadowMapCascades, uvw ).r;
				
				float currentDepth = projCoords.z;				
				shadow = currentDepth > depthSample ? 1.0 : 0.0;
				
				// Smoothly transition shadow to nothing in last cascade
				if (cascade == (elements-1))
				{
					// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
					const float transitionDistance = 30.0f;
					float transitionFactor = (_Lights.CSMSplits[cascade].range - distanceToCamera) / transitionDistance;
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
float CALCULATE_SHADOW_CSM_SOFT( vec3 P, int shadowMapIndex )
{
	float shadow = 0.0f;		
	if (shadowMapIndex >= 0)
	{
		float distanceToCamera = length(P - _Camera.pos);
		
		ivec3 texSize = textureSize( shadowMapCascades, 0 );		
		float elements = texSize[2];
		for (int cascade = 0; cascade < elements; ++cascade)
		{
			if (distanceToCamera < _Lights.CSMSplits[cascade].range)
			{
				vec4 lightSpace = VULKAN_CLIP * _Lights.CSMSplits[cascade].vp * vec4( P, 1 );
				vec3 projCoords = lightSpace.xyz / lightSpace.w;
				vec2 uv = projCoords.xy * 0.5 + 0.5;
				
				// Poisson sampling soft shadows
				float currentDepth = projCoords.z;
				
				float invisibleSamples = 0;
				for (int i=0; i<POISSON_DISK_SAMPLES; ++i){
					vec2 uvMod = uv + poissonDisk[i]/2048.0;
					vec3 uvw = vec3( uvMod, cascade );
					float depthSample = texture( shadowMapCascades, uvw ).r;
					
					if ( currentDepth > depthSample )
						invisibleSamples++;
				}	
				shadow = invisibleSamples / POISSON_DISK_SAMPLES;
				
				// Smoothly transition shadow to nothing in last cascade
				if (cascade == (elements-1))
				{
					// Transition factor: 0 to 1 from [SHADOW-DISTANCE-TRANSITION, SHADOW-DISTANCE]
					const float transitionDistance = 30.0f;
					float transitionFactor = (_Lights.CSMSplits[cascade].range - distanceToCamera) / transitionDistance;
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
vec4 VISUALIZE_CASCADES( vec3 P )
{
	vec4 colors[] = {
		vec4(1,0,0,1),
		vec4(0,1,0,1),
		vec4(0,0,1,1),
		vec4(1,1,0,1)
	};

	float distanceToCamera = length(P - _Camera.pos);
	
	ivec3 texSize = textureSize( shadowMapCascades, 0 );		
	float elements = texSize[2];
	for (int cascade = 0; cascade < elements; cascade++)
	{
		if (distanceToCamera < _Lights.CSMSplits[cascade].range)
			return colors[cascade];
	}	
	return vec4(1,1,1,1);
}

//**********************************************************************
// MISC FUNCTIONS
//**********************************************************************

//-----------------------------------------------
vec4 TO_LINEAR( vec4 color )
{
	return vec4( pow( color.r, GAMMA ), pow( color.g, GAMMA ),  pow( color.b, GAMMA ), color.a );
}
vec3 TO_LINEAR( vec3 color )
{
	return vec3( pow( color.r, GAMMA ), pow( color.g, GAMMA ),  pow( color.b, GAMMA ) );
}

//-----------------------------------------------
vec3 TO_SRGB( vec3 color )
{
	float gammaInv = 1.0/GAMMA;
	return vec3( pow( color.r, gammaInv ), pow( color.g, gammaInv ), pow( color.b, gammaInv ) ); 
}

//----------------------------------------------
float linearDepth( float depthSample )
{
    depthSample   = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * _Camera.zNear * _Camera.zFar / 
	                (_Camera.zFar + _Camera.zNear - depthSample * (_Camera.zFar - _Camera.zNear));
    return zLinear;
}