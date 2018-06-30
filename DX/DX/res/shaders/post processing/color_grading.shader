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

#include "/engine/shaders/includes/enginePS.hlsl"

cbuffer cbPerMaterial
{ 
	float contrast;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

Texture2D _MainTex;
SamplerState _Sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float2 uv = float2(fin.uv.x, fin.uv.y);
	float4 c = _MainTex.Sample(_Sampler0, uv);
	
	float3 result = (c.rgb - 0.5) * (1.0 + contrast) + 0.5;

	return float4(result.rgb, c.a);
}




