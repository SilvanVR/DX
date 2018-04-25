struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};


float4 main(FragmentIn fin) : SV_Target
{
	return fin.Color;
}