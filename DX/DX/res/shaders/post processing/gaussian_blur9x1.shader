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

cbuffer cbPerMaterial
{
	int 	horizontal;
	float 	blurScale;
};

Texture2D _MainTex;
SamplerState _Sampler0;

float4 main( FragmentIn fin ) : SV_Target
{
	const float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

	float width, height;
	_MainTex.GetDimensions(width, height);	

	float3 result = _MainTex.Sample(_Sampler0, fin.uv).rgb * weight[0];
	for(int i = 1; i < 5; ++i)
	{
		if (horizontal == 1)
		{
			float2 tex_offset = 1.0 / width * blurScale;
			result += _MainTex.Sample(_Sampler0, fin.uv + float2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += _MainTex.Sample(_Sampler0, fin.uv - float2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
		else
		{
			float2 tex_offset = 1.0 / height * blurScale;
			result += _MainTex.Sample(_Sampler0, fin.uv + float2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += _MainTex.Sample(_Sampler0, fin.uv - float2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	
	return float4(result, 1.0);
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
	int 	horizontal;
	float 	blurScale;
};

void main()
{
	const float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

	ivec2 texSize = textureSize( _MainTex, 0 );
	
	vec3 result = texture( _MainTex, inUV ).rgb * weight[0];
	for(int i = 1; i < 5; ++i)
	{
		if (horizontal == 1)
		{
			float width = texSize[0];
			vec2 tex_offset = vec2(1.0 / width * blurScale);
			result += texture( _MainTex, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture( _MainTex, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
		else
		{
			float height = texSize[1];
			vec2 tex_offset = vec2(1.0 / height * blurScale);
			result += texture( _MainTex, inUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture( _MainTex, inUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	
	outColor = vec4(result, 1.0);
}




