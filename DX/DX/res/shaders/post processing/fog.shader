// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#ZTest			Off

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VertexOut main( uint vI : SV_VERTEXID )
{
    VertexOut OUT;
	
	float2 uv = float2( (vI << 1) & 2, vI & 2 );
    OUT.uv   = float2( uv.x, 1-uv.y );
    OUT.PosH = float4( uv * 2.0f - 1.0f, 0.0f, 1.0f );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

cbuffer perMaterial
{
	float4 fogColor;
	float density;
	float gradient;
}

Texture2D _MainTex;
SamplerState _Sampler0;

Texture2D depthBuffer;
SamplerState depthBufferSampler;

//----------------------------------------------
float linearDepth(float depthSample)
{
    depthSample   = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * _zNear * _zFar / (_zFar + _zNear - depthSample * (_zFar - _zNear));
    return zLinear;
}

//----------------------------------------------
float4 main(FragmentIn fin) : SV_Target
{
	float4 sceneColor = _MainTex.Sample( _Sampler0, fin.uv );	
		
	float depth = depthBuffer.Sample( depthBufferSampler, fin.uv ).r;
	float linDepth = linearDepth( depth );
	
	float visibility = 1.0;
	visibility = clamp( exp( -pow( abs(linDepth*density), gradient ) ), 0.0, 1.0 );

	return lerp( fogColor, sceneColor, visibility );
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/version.glsl"

layout (location = 0) out vec2 outUV;

void main()
{
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}

//----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform sampler2D _MainTex;
layout (set = SET_FIRST, binding = 1) uniform sampler2D depthBuffer;
layout (set = SET_FIRST, binding = 2) uniform MATERIAL
{
	vec4 fogColor;
	float density;
	float gradient;
} material;

void main()
{
	vec4 sceneColor = texture( _MainTex, inUV );	
		
	float depth = texture( depthBuffer, inUV ).r;
	float linDepth = linearDepth( depth );
	
	float visibility = 1.0;
	visibility = clamp( exp( -pow( abs(linDepth*material.density), material.gradient ) ), 0.0, 1.0 );
	
	outColor = mix( material.fogColor, sceneColor, visibility );
}










