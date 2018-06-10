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

#include "/shaders/includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;	
	float2 uv : TEXCOORD0;
};

cbuffer perMaterial
{
	float4 fogColor;
	float density;
	float gradient;
}

Texture2D _MainTex;
SamplerState _Sampler0;

Texture2D depthBuffer;
SamplerState depthBufferSampler;

//----------------------------------------------
float linearDepth(float depthSample)
{
    depthSample   = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * _zNear * _zFar / (_zFar + _zNear - depthSample * (_zFar - _zNear));
    return zLinear;
}

//----------------------------------------------
float4 main(FragmentIn fin) : SV_Target
{
	float4 sceneColor = _MainTex.Sample( _Sampler0, fin.uv );	
		
	float depth = depthBuffer.Sample( depthBufferSampler, fin.uv ).r;
	float linDepth = linearDepth( depth );
	
	float visibility = 1.0;
	visibility = clamp( exp( -pow( abs(linDepth*density), gradient ) ), 0.0, 1.0 );
	
	return lerp( fogColor, sceneColor, visibility );	
	return float4( depth, depth, depth, sceneColor.a );
}




