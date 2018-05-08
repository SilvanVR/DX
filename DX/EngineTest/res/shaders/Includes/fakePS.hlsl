// This shader is only used to construct the correct constant buffers in code.
// Therefore it is required to include the file below and use EVERY BUFFER
// otherwise the shader compiler will not parse this buffer.
	
#include "res/shaders/includes/enginePS.hlsl"

float4 main(float4 PosH : SV_POSITION) : SV_Target
{
    return float4(1,1,1,1) * gTime;
}