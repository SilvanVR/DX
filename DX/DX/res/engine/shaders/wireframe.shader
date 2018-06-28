// ----------------------------------------------
#Fill			Wireframe
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
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE(vin.PosL);

    return OUT;
}

// ----------------------------------------------
#shader fragment

struct FragmentIn
{
    float4 PosH : SV_POSITION;
};

float4 main(FragmentIn fin) : SV_Target
{
    return float4(1,1,1,1);
}