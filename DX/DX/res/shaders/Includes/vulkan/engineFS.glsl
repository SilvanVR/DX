#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define ALPHA_THRESHOLD 0.1

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
layout (set = 1, binding = 0) uniform CAMERA
{
	mat4 view;
	mat4 proj;	
	float zNear;
	float zFar;
	vec3 pos;
} _Camera;