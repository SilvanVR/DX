// ----------------------------------------------
#Fill			Solid
#Cull 			None
#ZWrite 		Off

// ----------------------------------------------
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
    OUT.uv   = uv;
    OUT.PosH = float4( uv * 2.0f - 1.0f, 0.0f, 1.0f );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

cbuffer cbPerMaterial
{
	float4 tintColor;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

Texture2D _Main;
SamplerState _Sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 c = _Main.Sample(_Sampler0, fin.uv);
	return float4(c.g, c.r, c.b, c.a);
}