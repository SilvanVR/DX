// This shader is only used to construct the correct constant buffers in code.
// Therefore it is required to include the file below and use EVERY BUFFER
// otherwise the shader compiler will not parse this buffer.
// What exactly this shader does is not important, as long as it uses every buffer in 
// the file below.

#include "res/shaders/includes/engineVS.hlsl"

float4 main(float3 PosL : POSITION): SV_POSITION
{
    float4 res = TO_CLIP_SPACE(PosL) * gTime;

    return res;
}