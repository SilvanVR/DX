
cbuffer cbPerMaterial
{
	float4 	dir;
	float4 	color;
	float 	intensity;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Material : MATERIAL; // X Coord contains side-block, Y Top/Bottom block
	float4 WorldPos : POSITION;
};

Texture2DArray texArray;
SamplerState sampler0;

float3 fog(float3 color, float3 fcolor, float depth, float density)
{
    const float e = 2.71828182845904523536028747135266249;
    float f = pow(e, -pow(depth*density, 2));
    return lerp(fcolor, color, f);
}

float4 main(FragmentIn fin) : SV_Target
{	
	// Compute UV-Coordinates by projecting the world-pos along the respective axis
	float3 uvw;
	if (fin.Normal.x > 0.9 || fin.Normal.x < -0.9)
		uvw = float3( fin.WorldPos.zy,fin.Material.x );
	else if (fin.Normal.y > 0.9 || fin.Normal.y < -0.9)
		uvw = float3( fin.WorldPos.xz,fin.Material.y );
	else 
		uvw = float3( fin.WorldPos.xy,fin.Material.x );
	
	float4 textureColor = texArray.Sample(sampler0, uvw);
	
	// Phong Lighting
	float3 l = normalize(-dir.xyz);	
	float nDotL = dot(fin.Normal, l);
	
	float ambient = 0.4f;
	float3 diffuse = color.rgb * saturate(nDotL) * intensity;
	
	float3 finalColor = textureColor.rgb * ambient + textureColor.rgb * diffuse;
	return float4(finalColor, 1);
	
	//return float4( fog(finalColor, float3(0.8), depth, 0.005), 1 );
}










