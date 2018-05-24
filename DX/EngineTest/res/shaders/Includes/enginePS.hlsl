// Buffer keywords: (If the name of the buffer contains one of the strings then...) 
// "global": Per frame constant buffer.
// "object": Per object constant buffer.
// "camera": Per camera constant buffer.
// "light": Per light constant buffer.

#define MAX_LIGHTS 			16
#define DIRECTIONAL_LIGHT 	0
#define POINT_LIGHT 		1
#define SPOT_LIGHT 			2
#define SPECULAR_POWER		64

cbuffer cbPerCamera : register(b0)
{
    float4x4 gViewProj;
	float3 gCameraPos;
};

cbuffer cbBufferGlobal : register(b2)
{	
	float gTime;
	float gAmbient;
};

struct Light
{
    float3      position;               // 16 bytes
	int         lightType;              // 4 bytes
    //----------------------------------- (16 byte boundary)
    float3      direction;              // 12 bytes
	float 		intensity;				// 4 bytes
    //----------------------------------- (16 byte boundary)
    float4      color;                  // 16 bytes
    //----------------------------------- (16 byte boundary)
    float       spotAngle;              // 4 bytes
    float       range;					// 4 bytes
	float2		PADDING;				// 8 bytes	
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (4 * 16)
 

cbuffer cbBufferLights : register(b3)
{
	Light 	lights[MAX_LIGHTS];
	int 	lightCount;
};

//----------------------------------------------------------------------
// LIGHTING
//----------------------------------------------------------------------

//----------------------------------------------------------------------
float4 DoDiffuse( Light light, float3 L, float3 N )
{
    float NdotL = max( 0, dot( N, L ) );
    return light.color * NdotL * light.intensity;
}

//----------------------------------------------------------------------
float4 DoSpecular( Light light, float3 V, float3 L, float3 N )
{ 
    // Blinn-Phong lighting
    float3 H = normalize( L + V );
    float NdotH = max( 0, dot( N, H ) );
 
    return light.color * pow( NdotH, SPECULAR_POWER ) * light.intensity;
}

//----------------------------------------------------------------------
float DoAttenuation( Light light, float d )
{
    return max(0, 1 - smoothstep(0, light.range, d) );
}

//----------------------------------------------------------------------
// DIRECTIONAL LIGHT
//----------------------------------------------------------------------
float4 DoDirectionalLight( Light light, float3 V, float3 N )
{
    float3 L = -light.direction;
 
    float4 diffuse = DoDiffuse( light, L, N );
    float4 specular = DoSpecular( light, V, L, N );
 
    return diffuse + specular;
}

//----------------------------------------------------------------------
// POINT LIGHT
//----------------------------------------------------------------------
float4 DoPointLight( Light light, float3 V, float3 P, float3 N )
{
    float3 L = (light.position - P);
    float distance = length( L );
    L = L / distance;
 
    float attenuation = DoAttenuation( light, distance );
 
    float4 diffuse = DoDiffuse( light, L, N ) * attenuation;
    float4 specular = DoSpecular( light, V, L, N ) * attenuation;
 
    return diffuse + specular;
}

//----------------------------------------------------------------------
// SPOT LIGHT
//----------------------------------------------------------------------
float DoSpotCone( Light light, float3 L )
{
    float minCos = cos( light.spotAngle );
    float maxCos = ( minCos + 1.0f ) / 2.0f;
    float cosAngle = dot( light.direction, -L );
    return smoothstep( minCos, maxCos, cosAngle ); 
}

float4 DoSpotLight( Light light, float3 V, float3 P, float3 N )
{
    float3 L = (light.position - P);
    float distance = length( L );
    L = L / distance;
 
    float attenuation = DoAttenuation( light, distance );
    float spotIntensity = DoSpotCone( light, L );
 
    float4 diffuse = DoDiffuse( light, L, N ) * attenuation * spotIntensity;
    float4 specular = DoSpecular( light, V, L, N ) * attenuation * spotIntensity;
 
    return diffuse + specular;
}

//----------------------------------------------------------------------
// Calculates the lighting contribution for all lights in the light buffer.
// @Params:
//  "P": The position of the fragment in world space
//  "N": The normalized normal of the fragment in world space
//----------------------------------------------------------------------
float4 APPLY_LIGHTING( float3 P, float3 N )
{ 
	float3 V = normalize( gCameraPos - P ).xyz;
	
	float4 totalLight = { 0, 0, 0, 1 };
	
    [unroll]
	for (int i = 0; i < lightCount; i++)
	{
        switch( lights[i].lightType )
        {
        case DIRECTIONAL_LIGHT:
            totalLight += DoDirectionalLight( lights[i], V, N );
            break;
		case POINT_LIGHT:
            totalLight += DoPointLight( lights[i], V, P, N );
            break;
		case SPOT_LIGHT:		
            totalLight += DoSpotLight( lights[i], V, P, N );
            break;
        }
	}
	
	return totalLight;
}

//----------------------------------------------------------------------
// Applies lighting to a given fragment.
// @Params:
//  "fragColor": The color of the fragment which will receive the lighting
//  "P": The position of the fragment in world space
//  "N": The normalized normal of the fragment in world space
//----------------------------------------------------------------------
float4 APPLY_LIGHTING( float4 fragColor, float3 P, float3 N )
{ 
	float4 lighting = APPLY_LIGHTING( P, N );		
	return fragColor * gAmbient + fragColor * lighting; 
}