//----------------------------------------------
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

//----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

cbuffer cbPerMaterial
{
	float exposure;
};

Texture2D _MainTex;
SamplerState _Sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 hdrColor = _MainTex.Sample(_Sampler0, fin.uv);
	  
    // Exposure tone mapping
    float3 mapped = float3(1, 1, 1) - exp(-hdrColor.rgb * exposure);
	
    // Gamma correction 
    mapped = TO_SRGB(mapped);
	
	return float4(mapped, hdrColor.a);
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
layout (set = SET_FIRST, binding = 1) uniform MATERIAL
{
	float exposure;
};

void main()
{
	vec4 hdrColor = texture( _MainTex, inUV );
	  
    // Exposure tone mapping
    vec3 mapped = vec3(1, 1, 1) - exp(-hdrColor.rgb * exposure);
	
    // Gamma correction 
    mapped = TO_SRGB(mapped);
	
	outColor = vec4(mapped, hdrColor.a);
}





