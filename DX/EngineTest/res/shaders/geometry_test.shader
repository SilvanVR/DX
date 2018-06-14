// ----------------------------------------------
#Fill			Solid
#Cull 			Back
#ZWrite 		On
#ZTest 			Less
#Blend 			SrcAlpha OneMinusSrcAlpha
#Queue 			Geometry
#AlphaToMask 	Off

// ----------------------------------------------
#shader vertex

#include "includes/engineVS.hlsl"

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
#shader geometry

#include "/shaders/includes/engineGS.hlsl" 

struct VertexOut
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

static const float MAGNITUDE = 0.4;
void GenerateLine(VertexOut vertex, inout LineStream<GeoOut> OutputStream)
{
	GeoOut gout;
    gout.PosH = vertex.PosH;
    OutputStream.Append(gout);
    gout.PosH = vertex.PosH + float4(vertex.Normal, 0.0) * MAGNITUDE;
    OutputStream.Append(gout);
	OutputStream.RestartStrip();
}

[maxvertexcount(3)]
void main(triangle VertexOut input[3], inout TriangleStream<GeoOut> OutputStream)
{	
	//GenerateLine(input[0], OutputStream);
	//GenerateLine(input[1], OutputStream);
	//GenerateLine(input[2], OutputStream);
	
	float val = (sin(_Time)+1)/2;
    for(int i = 0; i < 3; i++)
    {
		GeoOut gout;
		gout.PosH = input[i].PosH + val * float4(input[i].Normal,1);
		gout.Normal = input[i].Normal;
        OutputStream.Append(gout);
    }
}

// ----------------------------------------------
#shader fragment

#include "includes/enginePS.hlsl"

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
};

float4 main(FragmentIn fin) : SV_Target
{
    return float4(fin.Normal,1);
}