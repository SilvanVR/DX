// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Queue 			Geometry
#DepthBias		0
#DBSlopeScaled  0
#DBClamp 		0

// ----------------------------------------------
#shader vertex

#include "/engine/shaders/includes/engineVS.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
	float2 tex : TEXCOORD0;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD0;
	float3 WorldPos : POSITION;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;

    OUT.PosH = TO_CLIP_SPACE( vin.PosL );
	OUT.Tex = vin.tex;
	OUT.WorldPos = TO_WORLD_SPACE( vin.PosL );
		
    return OUT;
}

// ----------------------------------------------
#shader fragment  

#include "/engine/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 WorldPos : POSITION;
};

Texture2D _MainTex;
SamplerState sampler0;

float main(FragmentIn fin) : SV_Depth
{
	float4 textureColor = _MainTex.Sample(sampler0, fin.Tex);
	if (textureColor.a < ALPHA_THRESHOLD)
		discard;
		
	float lightDistance = length(_CameraPos - fin.WorldPos);
	// Map to [0,1] range
	lightDistance /= _zFar;
	return lightDistance;
}