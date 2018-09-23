#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

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

// Vulkan clip space has inverted Y and half Z. This fixes it.
const mat4 VULKAN_CLIP = { { 1.0,  0.0, 0.0, 0.0 },
                           { 0.0, -1.0, 0.0, 0.0 },
                           { 0.0,  0.0, 0.5, 0.0 },
                           { 0.0,  0.0, 0.0, 1.0 } };