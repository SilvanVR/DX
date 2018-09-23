// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			Transparent
#AlphaToMask 	Off

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float2 Size : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 WorldPos : POSITION;
	float2 Size : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE( vin.PosL );
	OUT.WorldPos = TO_WORLD_SPACE( vin.PosL );
	OUT.Size = vin.Size;
	
    return OUT;
}

// ----------------------------------------------
#shader geometry

#include "/engine/shaders/includes/engineGS.hlsl" 

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 WorldPos : POSITION;
	float2 Size : TEXCOORD;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
};

// This shader expands a single point into a quad using the uv-coords as a size param.
// The point is the center of the quad.
[maxvertexcount(4)]
void main(point VertexOut input[1], inout TriangleStream<GeoOut> OutputStream)
{	
	float3 normal = normalize( _CameraPos - input[0].WorldPos );
	float3 up = float3(0,1,0);
	float3 right = normalize( cross( normal, up ) );
	up = normalize( cross( right, normal ) );
	
	float halfWidth = input[0].Size.x * 0.5f;
	float halfHeight = input[0].Size.y * 0.5f;
	
	float3 upVector = halfHeight * up;
	float3 rightVector = halfWidth * right;
	
	float4 v[4];
	v[0] = float4(input[0].WorldPos + rightVector - upVector, 1.0f);
	v[1] = float4(input[0].WorldPos - rightVector - upVector, 1.0f);
	v[2] = float4(input[0].WorldPos + rightVector + upVector, 1.0f);
	v[3] = float4(input[0].WorldPos - rightVector + upVector, 1.0f);

	GeoOut gout;
	gout.Normal = normal;
	
	gout.PosH = mul( _ViewProj, v[0] );
	gout.UV = float2( 1, 1 );
	OutputStream.Append( gout );
    
	gout.PosH = mul( _ViewProj, v[1] );
	gout.UV = float2( 0, 1 );
	OutputStream.Append( gout );
    
	gout.PosH = mul( _ViewProj, v[2] );
	gout.UV = float2( 1, 0 );
	OutputStream.Append( gout );
	
	gout.PosH = mul( _ViewProj, v[3] );
	gout.UV = float2( 0, 0 );
	OutputStream.Append( gout );
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
};

Texture2D tex; 
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
    return tex.Sample( sampler0, fin.UV );
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;
layout (location = 1) in vec2 VERTEX_UV;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec2 outSize;

void main()
{
    gl_Position = TO_CLIP_SPACE( VERTEX_POSITION );
	outWorldPos = TO_WORLD_SPACE( VERTEX_POSITION );
	outSize 	= VERTEX_UV;
}

// ----------------------------------------------
#shader geometry

#include "/engine/shaders/includes/vulkan/engineGS.glsl" 

layout (location = 0) in vec3 inWorldPos[];
layout (location = 1) in vec2 inSize[];

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;

// This shader expands a single point into a quad using the uv-coords as a size param.
// The point is the center of the quad.
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

void main()
{	
	vec3 normal = normalize( _Camera.pos - inWorldPos[0] );
	vec3 up = vec3(0,1,0);
	vec3 right = normalize( cross( normal, up ) );
	up = normalize( cross( right, normal ) );
	
	float halfWidth = inSize[0].x * 0.5f;
	float halfHeight = inSize[0].y * 0.5f;
	
	vec3 upVector = halfHeight * up;
	vec3 rightVector = halfWidth * right;
	
	vec4 v[4];
	v[0] = vec4(inWorldPos[0] + rightVector - upVector, 1.0f);
	v[1] = vec4(inWorldPos[0] - rightVector - upVector, 1.0f);
	v[2] = vec4(inWorldPos[0] + rightVector + upVector, 1.0f);
	v[3] = vec4(inWorldPos[0] - rightVector + upVector, 1.0f);

	outNormal = normal;
	
	mat4 viewProj = VULKAN_CLIP * _Camera.proj * _Camera.view;
	gl_Position  = viewProj * v[0];
	outUV = vec2( 1, 1 );
    EmitVertex();
    
	gl_Position  = viewProj * v[1];
	outUV = vec2( 0, 1 );
    EmitVertex();
    
	gl_Position  = viewProj * v[2];
	outUV = vec2( 1, 0 );
    EmitVertex();
	
	gl_Position  = viewProj * v[3];
	outUV = vec2( 0, 0 );
    EmitVertex();
	
    EndPrimitive();
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform sampler2D tex;

void main()
{
    outColor = texture( tex, inUV );
}