// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZWrite 		Off
#Queue 			Background

// ----------------------------------------------
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

TextureCube<float4> environmentMap;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float3 N = normalize( fin.UVW );

    float3 irradiance = float3( 0,0,0 );   
    
    float3 up    = float3( 0.0, 1.0, 0.0 );
    float3 right = cross( up, N );
    up         	 = cross( N, right );
       
    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // Spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			
            // Tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

			float4 textureColor = environmentMap.Sample( sampler0, sampleVec );
            irradiance += textureColor.rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

	return float4( irradiance, 1.0 );
}