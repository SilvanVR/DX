// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZTest			Off
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
    float3 tex : TEXCOORD0;
};

VertexOut main(VertexIn vin)
{
    VertexOut OUT;
	
	OUT.tex = vin.PosL;

    float4 clipPos = TO_CLIP_SPACE( float4(vin.PosL, 0.0f) );
	
	// Little trick which places the object very close to the far-plane
	OUT.PosH = clipPos.xyww;
	OUT.PosH.z *= 0.999999;
	
    return OUT;
}

// ----------------------------------------------
#shader fragmentf

#include "includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 tex : TEXCOORD0;
};

TextureCube<float4> Cubemap;
SamplerState sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 textureColor = Cubemap.Sample(sampler0, normalize(fin.tex));
	
	// This is a quick hack to let the directional light influence the skybox
	float3 V = float3(0,0,0);
	float3 N = float3(0,1,0);
	
	float4 totalLight = { 0, 0, 0, 1 };	
	for (int i = 0; i < lightCount; i++)
	{
        switch( lights[i].lightType )
        {
        case DIRECTIONAL_LIGHT:
            totalLight += DoDirectionalLight( lights[i], V, N );
            break;
        }
	}
	
	return textureColor * gAmbient + textureColor * totalLight;
}