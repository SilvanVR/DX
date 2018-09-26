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
    OUT.uv   = uv;
    OUT.PosH = float4( uv * 2.0f - 1.0f, 0.0f, 1.0f );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

#define PI 3.14159265359f

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

//----------------------------------------------------------------------
float GeometrySchlickGGX( float NdotV, float roughness )
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

//----------------------------------------------------------------------
float GeometrySmith( float3 N, float3 V, float3 L, float roughness )
{
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotL = max( dot( N, L ), 0.0 );
    float ggx2  = GeometrySchlickGGX( NdotV, roughness );
    float ggx1  = GeometrySchlickGGX( NdotL, roughness );
	
    return ggx1 * ggx2;
}

//----------------------------------------------------------------------------
float RadicalInverse_VdC( uint bits ) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

//----------------------------------------------------------------------------
float2 Hammersley( uint i, uint N )
{
	return float2( float(i)/float(N), RadicalInverse_VdC(i) );
}

//----------------------------------------------------------------------------
float3 ImportanceSampleGGX( float2 Xi, float3 N, float roughness )
{
	float a = roughness * roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt( (1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y) );
	float sinTheta = sqrt( 1.0 - cosTheta*cosTheta );
	
	// From spherical coordinates to cartesian coordinates - halfway vector
	float3 H;
	H.x = cos( phi ) * sinTheta;
	H.y = sin( phi ) * sinTheta;
	H.z = cosTheta;
	
	// From tangent-space H vector to world-space sample vector
	float3 up        = abs( N.z ) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
	float3 tangent   = normalize( cross( up, N ) );
	float3 bitangent = cross( N, tangent );
	
	float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize( sampleVec );
}

//----------------------------------------------------------------------------
float2 IntegrateBRDF( float NdotV, float roughness )
{
    float3 V;
    V.x = sqrt( 1.0 - NdotV * NdotV );
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    float3 N = float3( 0.0, 0.0, 1.0 );

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley( i, SAMPLE_COUNT );
        float3 H  = ImportanceSampleGGX( Xi, N, roughness );
        float3 L  = normalize( 2.0 * dot(V, H) * H - V );

        float NdotL = max( L.z, 0.0 );
        float NdotH = max( H.z, 0.0 );
        float VdotH = max( dot(V, H), 0.0 );

        if (NdotL > 0.0)
        {
            float G = GeometrySmith( N, V, L, roughness );
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow( 1.0 - VdotH, 5.0 );

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float( SAMPLE_COUNT );
    B /= float( SAMPLE_COUNT );
    return float2( A, B );
}


//----------------------------------------------------------------------------
float2 main( FragmentIn fin ) : SV_Target
{
    float2 integratedBRDF = IntegrateBRDF( fin.uv.x, fin.uv.y );
    return integratedBRDF;
}


//----------------------------------------------
// Vulkan
//----------------------------------------------
#vulkan
#shader vertex

#include "/engine/shaders/includes/vulkan/engineFS.glsl"

layout (location = 0) out vec2 outUV;

void main()
{
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}
 
// ----------------------------------------------
#shader fragment

#include "/engine/shaders/includes/vulkan/version.glsl"

#define PI 3.14159265359

// In data
layout (location = 0) in vec2 inUV;

// Out data
layout (location = 0) out vec2 outColor; 


//----------------------------------------------------------------------
float GeometrySchlickGGX( float NdotV, float roughness )
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

//----------------------------------------------------------------------
float GeometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
{
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotL = max( dot( N, L ), 0.0 );
    float ggx2  = GeometrySchlickGGX( NdotV, roughness );
    float ggx1  = GeometrySchlickGGX( NdotL, roughness );
	
    return ggx1 * ggx2;
}

//----------------------------------------------------------------------------
float RadicalInverse_VdC( uint bits ) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

//----------------------------------------------------------------------------
vec2 Hammersley( uint i, uint N )
{
	return vec2( float(i)/float(N), RadicalInverse_VdC(i) );
}

//----------------------------------------------------------------------------
vec3 ImportanceSampleGGX( vec2 Xi, vec3 N, float roughness )
{
	float a = roughness * roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt( (1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y) );
	float sinTheta = sqrt( 1.0 - cosTheta*cosTheta );
	
	// From spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos( phi ) * sinTheta;
	H.y = sin( phi ) * sinTheta;
	H.z = cosTheta;
	
	// From tangent-space H vector to world-space sample vector
	vec3 up        = abs( N.z ) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize( cross( up, N ) );
	vec3 bitangent = cross( N, tangent );
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize( sampleVec );
}

//----------------------------------------------------------------------------
vec2 IntegrateBRDF( float NdotV, float roughness )
{
    vec3 V;
    V.x = sqrt( 1.0 - NdotV * NdotV );
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3( 0.0, 0.0, 1.0 );

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley( i, SAMPLE_COUNT );
        vec3 H  = ImportanceSampleGGX( Xi, N, roughness );
        vec3 L  = normalize( 2.0 * dot(V, H) * H - V );

        float NdotL = max( L.z, 0.0 );
        float NdotH = max( H.z, 0.0 );
        float VdotH = max( dot(V, H), 0.0 );

        if (NdotL > 0.0)
        {
            float G = GeometrySmith( N, V, L, roughness );
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow( 1.0 - VdotH, 5.0 );

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float( SAMPLE_COUNT );
    B /= float( SAMPLE_COUNT );
    return vec2( A, B );
}

void main()
{
    outColor = IntegrateBRDF( inUV.x, inUV.y );
}