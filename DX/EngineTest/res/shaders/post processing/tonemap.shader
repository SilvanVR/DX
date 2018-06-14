//----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZWrite 		Off

//----------------------------------------------
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
    OUT.uv   = float2( uv.x, 1-uv.y );
    OUT.PosH = float4( uv * 2.0f - 1.0f, 0.0f, 1.0f );
	
    return OUT;
}

//----------------------------------------------
#shader fragment

#include "/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

cbuffer cbPerMaterial
{
	float exposure;
};

Texture2D _MainTex;
SamplerState _Sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 hdrColor = _MainTex.Sample(_Sampler0, fin.uv);
	  
    // Exposure tone mapping
    float3 mapped = float3(1, 1, 1) - exp(-hdrColor.rgb * exposure);
	
    // Gamma correction 
    mapped = TO_SRGB(mapped);
	
	return float4(mapped, hdrColor.a);
}




