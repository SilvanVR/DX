// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.

cbuffer cbBufferGlobal : register(b2)
{	
	float gTime;
};

//----------------------------------------------------------------------
float3 APPLY_LIGHTING(float4 fragPos, float3 normal)
{
	// Loop over each light in buffer and calculate lighting
    return float3(1,0,0);
}