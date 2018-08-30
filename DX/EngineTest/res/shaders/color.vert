#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Predefined vertex streams
#define VERTEX_POSITION POSITION
#define VERTEX_COLOR	COLOR
#define VERTEX_UV		TEXCOORD
#define VERTEX_NORMAL	NORMAL
#define VERTEX_TANGENT	TANGENT

// PER OBJECT PUSH CONSTANT
layout (std140, push_constant) uniform PushConstant 
{
	mat4 world;
} _Object;

//----------------------------------------------------------------------
// Descriptor-Sets
//----------------------------------------------------------------------

// Per camera ubo
layout (set = 0, binding = 0) uniform CAMERA
{
	mat4 mvp;
	vec4 pad0;
	vec4 pad1;
	vec4 pad2;
} _Camera;

//----------------------------------------------------------------------
vec4 TO_CLIP_SPACE( vec4 vert )
{
    return _Camera.mvp * vert;
}

//----------------------------------------------------------------------
vec4 TO_CLIP_SPACE( vec3 vert )
{
    return TO_CLIP_SPACE( vec4( vert, 1 ) );
}

layout (location = 0) in vec3 VERTEX_POSITION;

void main()
{
	gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
}