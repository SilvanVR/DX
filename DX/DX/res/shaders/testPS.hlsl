
cbuffer cbPerMaterial
{
	float4 pixelColor;
	float pixelVal;
};

struct FragmentIn
{
    float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(FragmentIn fin) : SV_Target
{
	float test = pixelVal;
	//return float4(fin.Color.r,1,0,1);
    //return float4(1, 1, 1, 1.0) * pixelVal;
    //return float4(pixelVal, 1, 1, 1) ;
    return float4(pixelVal, 1-pixelColor.g, fin.Color.r, 1.0);
}