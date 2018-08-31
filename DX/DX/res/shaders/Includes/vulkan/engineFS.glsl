#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_LIGHTS 					16
#define ALPHA_THRESHOLD 			0.1

//----------------------------------------------------------------------
// Descriptor-Sets
//----------------------------------------------------------------------

// Per frame global ubo
layout (set = 0, binding = 0) uniform GLOBAL
{
	float time;
	float ambient;
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
    vec4      	position;               // 16 bytes
	int         lightType;              // 4 bytes
    //----------------------------------- (16 byte boundary)
    vec4      	direction;              // 12 bytes
	float 		intensity;				// 4 bytes
    //----------------------------------- (16 byte boundary)
    vec4      color;                  // 16 bytes
    //----------------------------------- (16 byte boundary)
    float       spotAngle;              // 4 bytes
    float       range;					// 4 bytes
    int       	shadowMapIndex;			// 4 bytes
	int			shadowType;				// 4 bytes	
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (4 * 16)
 
// Light ubo
layout (set = 0, binding = 2) uniform LIGHTS
{
	Light 	lights[MAX_LIGHTS];
	int 	count; // 4 bytes
} _Lights;