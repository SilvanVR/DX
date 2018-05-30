// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZWrite 		Off
#Priority 		Geometry

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