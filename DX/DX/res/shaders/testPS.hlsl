
struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(FragmentIn fin) : SV_Target
{
   //greturn float4(1, 0, 0, 0.5);
   //return fin.Color;
   return fin.Color;
}