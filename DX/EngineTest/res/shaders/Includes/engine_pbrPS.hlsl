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
#define PI					3.14159265359

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
    return max( 1 - smoothstep(0, light.range, d), 0 );
}

//----------------------------------------------------------------------
float3 fresnelSchlickRoughness( float cosTheta, float3 F0, float roughness )
{
	float oneMinusRoughness = 1.0 - roughness;
    return F0 + ( max( float3(oneMinusRoughness, oneMinusRoughness, oneMinusRoughness), F0 ) - F0) * pow( 1.0 - cosTheta, 5.0 );
}

//----------------------------------------------------------------------
float DistributionGGX( float3 N, float3 H, float roughness )
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max( dot( N, H ), 0.0 );
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

//----------------------------------------------------------------------
float GeometrySchlickGGX( float NdotV, float roughness )
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

//----------------------------------------------------------------------
float GeometrySmith( float3 N, float3 V, float3 L, float roughness )
{
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotL = max( dot( N, L ), 0.0 );
    float ggx2  = GeometrySchlickGGX( NdotV, roughness );
    float ggx1  = GeometrySchlickGGX( NdotL, roughness );
	
    return ggx1 * ggx2;
}

//----------------------------------------------------------------------
float3 calcLight( Light light, float3 radiance, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic )
{	
	float3 F0 = float3( 0.04, 0.04, 0.04 ); 
	F0      = lerp( F0, albedo, metallic );
	float3 F = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
	float3 kS = F;
	float3 kD = float3( 1,1,1 ) - kS;
  
	kD *= 1.0 - metallic;	

	float3 L = normalize( light.position - P );
    float3 H = normalize( V + L );
		
	float NDF = DistributionGGX( N, H, roughness );       
	float G   = GeometrySmith( N, V, L, roughness ); 
		
	float3 nominator  = NDF * G * F;
	float denominator = 4 * max( dot( V, N ), 0.0) * max( dot( L, N ), 0.0) + 0.001; 
	float3 specular   = nominator / denominator;  
  
    float NdotL = max( dot( N, L ), 0.0 );        
    float3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	return Lo;  
}


//----------------------------------------------------------------------
// DIRECTIONAL LIGHT
//----------------------------------------------------------------------
float3 DoDirectionalLight( Light light, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic )
{
    float3 radiance = light.color.rgb * light.intensity;
    return calcLight( light, radiance, albedo, V, P, N, roughness, metallic );
}

//----------------------------------------------------------------------
// POINT LIGHT
//----------------------------------------------------------------------
float3 DoPointLight( Light light, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic  )
{  
    float distance  = length( light.position - P );
    float3 radiance = light.color.rgb * DoAttenuation( light, distance ) * light.intensity; 
 
    return calcLight( light, radiance, albedo, V, P, N, roughness, metallic );
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

float3 DoSpotLight(  Light light, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic )
{    
	float3 L = (light.position - P);
	float distance  = length( light.position - P );
    float3 radiance = light.color.rgb * DoAttenuation( light, distance ) * light.intensity * DoSpotCone( light, L ); 
 
    return calcLight( light, radiance, albedo, V, P, N, roughness, metallic );
}

//----------------------------------------------------------------------
// Applies lighting to a given fragment.
// @Params:
//  "fragColor": The color of the fragment which will receive the lighting
//  "P": The position of the fragment in world space
//  "N": The normal of the fragment in world space
//  "roughness": The roughness of the fragment
//  "metallic": Metallic of the fragment
//----------------------------------------------------------------------
float4 APPLY_LIGHTING( float4 fragColor, float3 P, float3 normal, float roughness, float metallic )
{ 
	float3 V = normalize( gCameraPos - P ).xyz;
	float3 N = normalize( normal );
	
	float3 Lo = { 0, 0, 0 };
	
    [unroll]
	for (int i = 0; i < lightCount; i++)
	{
        switch( lights[i].lightType )
        {
        case DIRECTIONAL_LIGHT:
            Lo += DoDirectionalLight( lights[i], fragColor.rgb, V, P, N, roughness, metallic );
            break;
		case POINT_LIGHT:		
            Lo += DoPointLight( lights[i], fragColor.rgb, V, P, N, roughness, metallic );
            break;
		case SPOT_LIGHT:
			Lo += DoSpotLight( lights[i], fragColor.rgb, V, P, N, roughness, metallic );
            break;
        }
	}
	
	float3 ambient = fragColor.rgb * gAmbient;
	float3 color = ambient + fragColor.rgb * Lo;
	color = color / (color + float3(1,1,1));
	color = pow( color, float3(1.0/2.2,1.0/2.2,1.0/2.2) ); 
	
	return float4(color,1); 
}