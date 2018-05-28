// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off
#Queue 			Background

// ----------------------------------------------
#shader vertex

#include "includes/engineVS.hlsl"

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

#include "includes/enginePS.hlsl"

#define NUM_SAMPLES 1024

cbuffer cbPerMaterial
{ 
	float roughness;
	float resolution; // resolution of source cubemap (per face)
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float3 UVW : POSITION;
};

TextureCube<float4> environmentMap;
SamplerState sampler0;

//----------------------------------------------------------------------------
float DistributionGGX( float3 N, float3 H, float roughness )
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
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
	float a = roughness*roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// From spherical coordinates to cartesian coordinates - halfway vector
	float3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// From tangent-space H vector to world-space sample vector
	float3 up        = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
	float3 tangent   = normalize(cross(up, N));
	float3 bitangent = cross(N, tangent);
	
	float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize( sampleVec );
}

//----------------------------------------------------------------------------
float4 main( FragmentIn fin ) : SV_Target
{
    float3 N = normalize( fin.UVW );
    
    // make the simplifying assumption that V equals R equals the normal 
    float3 R = N;
    float3 V = R;

    float3 prefilteredColor = float3(0,0,0);
    float totalWeight = 0.0;

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        // Generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        float2 Xi = Hammersley( i, NUM_SAMPLES );
        float3 H  = ImportanceSampleGGX( Xi, N, roughness );
        float3 L  = normalize( 2.0 * dot( V, H ) * H - V );

        float NdotL = clamp(dot(N, L), 0.0, 1.0);
        if(NdotL > 0.0)
        {
            // Sample from the environment's mip level based on roughness/pdf
            float D   = DistributionGGX( N, H, roughness );
            float NdotH = max( dot( N, H ), 0.0 );
            float HdotV = max( dot( H, V ), 0.0 );
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(NUM_SAMPLES) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
			float4 textureColor = environmentMap.SampleLevel( sampler0, L, mipLevel );
            prefilteredColor += textureColor.rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    return float4( prefilteredColor, 1.0 );
}
