// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZWrite 		Off
#Priority 		Geometry

//----------------------------------------------
// D3D11
//----------------------------------------------
#d3d11
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;	
	float3 UVW : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    float4 clipPos = TO_CLIP_SPACE( float4( vin.PosL, 0.0f ) );
	
	// Little trick which places the object very close to the far-plane
	OUT.PosH = clipPos.xyww;
	OUT.PosH.z *= 0.99999;	
	OUT.UVW = vin.PosL;
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 UVW : POSITION;
};

Texture2D equirectangularMap;
SamplerState sampler0;

//----------------------------------------------
float2 SampleSphericalMap( float3 v )
{
    float2 uv = float2( atan2( v.z, v.x ), asin( v.y ) );
	
	const float2 invAtan = float2( 0.1591, 0.3183 );
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//----------------------------------------------
float4 main( FragmentIn fin ) : SV_Target
{ 
	float2 uv = SampleSphericalMap( normalize( fin.UVW ).xyz );
	float2 uvFlipped = float2( uv.x, 1-uv.y );
    float3 color = equirectangularMap.Sample( sampler0, uvFlipped ).rgb;
    
	return float4( color , 1.0 );
}

//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineVS.glsl"

layout (location = 0) in vec3 VERTEX_POSITION;

layout (location = 0) out vec3 outUVW;

void main()
{
	outUVW = VERTEX_POSITION;
	
    vec4 clip = TO_CLIP_SPACE( vec4( VERTEX_POSITION, 0.0f ) );
	
	// Little trick which places the object very close to the far-plane
	clip = clip.xyww;
	clip.z *= 0.99999;
	gl_Position = clip;
}

// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) in vec3 inUVW;

layout (location = 0) out vec4 outColor;

layout (set = SET_FIRST, binding = 0) uniform sampler2D equirectangularMap;

//----------------------------------------------
vec2 SampleSphericalMap( vec3 v )
{
    vec2 uv = vec2( atan( v.z, v.x ), asin( v.y ) );
	
	const vec2 invAtan = vec2( 0.1591, 0.3183 );
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

//----------------------------------------------
void main()
{ 
	vec2 uv = SampleSphericalMap( normalize( inUVW ) );
	vec2 uvFlipped = vec2( uv.x, 1-uv.y );
    vec3 color = texture( equirectangularMap, uvFlipped ).rgb;
    
	outColor = vec4( color , 1.0 );
}