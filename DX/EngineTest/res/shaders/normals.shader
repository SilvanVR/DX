// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float3 normal : NORMAL;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH   = TO_CLIP_SPACE( vin.PosL );
	OUT.Normal = TRANSFORM_NORMAL( vin.normal ); 					
	
    return OUT;
}

// ----------------------------------------------
#shader fragment    

#include "/engine/shaders/includes/enginePS.hlsl" 

struct FragmentIn  
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

//----------------------------------------------------------------------
float4 main(FragmentIn fin) : SV_Target
{
	return float4(fin.Normal,1); 
}