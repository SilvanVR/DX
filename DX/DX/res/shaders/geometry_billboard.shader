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