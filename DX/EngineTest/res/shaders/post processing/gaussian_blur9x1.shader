// ----------------------------------------------
#Fill			Solid
#Cull 			Front
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
    OUT.uv   = float2( uv.x, 1-uv.y );
    OUT.PosH = float4( uv * 2.0f - 1.0f, 0.0f, 1.0f );
	
    return OUT;
}

// ----------------------------------------------
#shader fragment

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

cbuffer cbPerMaterial
{
	int 	horizontal;
	float 	blurScale;
};

Texture2D _MainTex;
SamplerState _Sampler0;

float4 main( FragmentIn fin ) : SV_Target
{
	const float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

	float width, height;
	_MainTex.GetDimensions(width, height);	

	float3 result = _MainTex.Sample(_Sampler0, fin.uv).rgb * weight[0];
	for(int i = 1; i < 5; ++i)
	{
		if (horizontal == 1)
		{
			float2 tex_offset = 1.0 / width * blurScale;
			result += _MainTex.Sample(_Sampler0, fin.uv + float2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += _MainTex.Sample(_Sampler0, fin.uv - float2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
		else
		{
			float2 tex_offset = 1.0 / height * blurScale;
			result += _MainTex.Sample(_Sampler0, fin.uv + float2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += _MainTex.Sample(_Sampler0, fin.uv - float2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	
	return float4(result, 1.0);
}

