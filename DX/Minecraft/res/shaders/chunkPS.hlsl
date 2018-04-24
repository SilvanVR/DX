
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

float4 main(FragmentIn fin) : SV_Target
{	
	// Compute UV-Coordinates by projecting the world-pos along the respective axis
	float3 uvw;
	if (fin.Normal.x > 0.9 || fin.Normal.x < -0.9)
		uvw = float3( -fin.WorldPos.zy + 0.5, fin.Material.x );
	else if (fin.Normal.y > 0.9 || fin.Normal.y < -0.9)
		uvw = float3( fin.WorldPos.xz + 0.5,fin.Material.y );
	else 
		uvw = float3( -fin.WorldPos.xy + 0.5, fin.Material.x );
	
	float4 textureColor = texArray.Sample(sampler0, uvw);
	
	// Phong Lighting
	float3 l = normalize(-dir.xyz);	
	float nDotL = dot(fin.Normal, l);
	
	float ambient = 0.4f;
	float3 diffuse = color.rgb * saturate(nDotL) * intensity;
	
	float3 finalColor = textureColor.rgb * ambient + textureColor.rgb * diffuse;
	return float4(finalColor, textureColor.a);
}










