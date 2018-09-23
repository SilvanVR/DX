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