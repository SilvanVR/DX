
#define SPECULAR_POWER 64

//----------------------------------------------------------------------
// LIGHTING (Blinn-Phong)
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
    float3 H = normalize( L + V );
    float NdotH = max( 0, dot( N, H ) );
 
    return light.color * pow( NdotH, SPECULAR_POWER ) * light.intensity;
}

//----------------------------------------------------------------------
float DoAttenuation( Light light, float d )
{
    return max( 0, 1 - smoothstep(0, light.range, d) );
}

//----------------------------------------------------------------------
// DIRECTIONAL LIGHT
//----------------------------------------------------------------------
float4 DoDirectionalLight( Light light, float3 V, float3 P, float3 N )
{
    float3 L = -light.direction;
 
    float4 diffuse = DoDiffuse( light, L, N );
    //float4 specular = DoSpecular( light, V, L, N );
		 
	float shadow = CALCULATE_SHADOW_DIR( P, light.range, light.shadowMapIndex );

    return diffuse * shadow;
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
 
	float shadow = CALCULATE_SHADOW_3D( P, light.position, light.range, light.shadowMapIndex ) * attenuation;
 
    return (diffuse + specular) * shadow;
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
 
	// Attenuation must be multiplied with the shadow
	float shadow = CALCULATE_SHADOW_2D( P, light.shadowMapIndex ) * attenuation;
 
    return (diffuse + specular) * shadow;
}

//----------------------------------------------------------------------
// Calculates the lighting contribution for all lights in the light buffer.
// @Params:
//  "P": The position of the fragment in world space
//  "N": The normal of the fragment in world space
//----------------------------------------------------------------------
float4 APPLY_LIGHTING( float3 P, float3 N )
{ 
	float3 V = normalize( _CameraPos - P ).xyz;
	float3 normal = normalize( N );
	
	float4 totalLight = { 0, 0, 0, 1 };
	
    [unroll]
	for (int i = 0; i < _LightCount; i++)
	{
        switch( _Lights[i].lightType )
        {
        case DIRECTIONAL_LIGHT:
            totalLight += DoDirectionalLight( _Lights[i], V, P, normal );
            break;
		case POINT_LIGHT:
            totalLight += DoPointLight( _Lights[i], V, P, normal );
            break;
		case SPOT_LIGHT:		
            totalLight += DoSpotLight( _Lights[i], V, P, normal );
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
//  "N": The normal of the fragment in world space
//----------------------------------------------------------------------
float4 APPLY_LIGHTING( float4 fragColor, float3 P, float3 N )
{ 
	float4 lighting = APPLY_LIGHTING( P, N );		
	return fragColor * _Ambient + fragColor * lighting; 
}