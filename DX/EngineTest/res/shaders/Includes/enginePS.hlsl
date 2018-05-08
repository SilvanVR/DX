cbuffer cbBufferGlobal : register(b2)
{	
	float4 gColor;
    float gTime;
};

//----------------------------------------------------------------------
float3 APPLY_LIGHTING(float4 fragPos, float3 normal)
{
	// Loop over each light in buffer and calculate lighting
    return float3(1,0,0);
}