// ----------------------------------------------
#Fill			Solid
#Cull 			Front
#ZWrite 		Off
#Queue 			Geometry

// ----------------------------------------------
#shader vertex

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VertexOut main(uint vI : SV_VERTEXID)
{
    VertexOut OUT;

	float2 uv = float2((vI << 1) & 2, vI & 2);      
	OUT.UV = float2(uv.x, 1-uv.y);                  
	OUT.PosH = float4(uv * 2.0f - 1.0f, 0.0f, 1.0f);
	
	return OUT;
}

// ----------------------------------------------
#shader fragment

struct FragmentIn
{
    float4 PosH : SV_POSITION;
    float2 UV : TEXCOORD0;
};

Texture2D _MainTex;    
SamplerState _Sampler0;

float4 main(FragmentIn fin) : SV_Target
{
	float4 color = _MainTex.Sample(_Sampler0, fin.UV);
    return color;
}