
#define SPECULAR_POWER 64

//----------------------------------------------------------------------
// LIGHTING (Blinn-Phong)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
vec4 DoDiffuse( Light light, vec3 L, vec3 N )
{
    float NdotL = max( 0, dot( N, L ) );
    return light.color * NdotL * light.intensity;
}

//----------------------------------------------------------------------
vec4 DoSpecular( Light light, vec3 V, vec3 L, vec3 N )
{ 
    vec3 H = normalize( L + V );
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
vec4 DoDirectionalLight( Light light, vec3 V, vec3 P, vec3 N )
{
    vec3 L = -light.direction;
 
    vec4 diffuse = DoDiffuse( light, L, N );
    vec4 specular = DoSpecular( light, V, L, N );
		 
	// float shadow = 1.0;
	// if (light.shadowType == SHADOW_TYPE_HARD)
		// shadow = CALCULATE_SHADOW_DIR( P, light.range, light.shadowMapIndex );
	// else if (light.shadowType == SHADOW_TYPE_SOFT)
		// shadow = CALCULATE_SHADOW_DIR_SOFT( P, light.range, light.shadowMapIndex );
	// else if (light.shadowType == SHADOW_TYPE_CSM)
		// shadow = CALCULATE_SHADOW_CSM( P, light.shadowMapIndex );
	// else if (light.shadowType == SHADOW_TYPE_CSM_SOFT)
		// shadow = CALCULATE_SHADOW_CSM_SOFT( P, light.shadowMapIndex );
		
    return (diffuse + specular) ;
}

//----------------------------------------------------------------------
// POINT LIGHT
//----------------------------------------------------------------------
vec4 DoPointLight( Light light, vec3 V, vec3 P, vec3 N )
{
    vec3 L = (light.position - P);
    float dist = length( L );
    L = L / dist;
 
    float attenuation = DoAttenuation( light, dist );
 
    vec4 diffuse = DoDiffuse( light, L, N ) * attenuation;
    vec4 specular = DoSpecular( light, V, L, N ) * attenuation;
 
	// float shadow = 1.0;
	// if (light.shadowType == SHADOW_TYPE_HARD)
		// shadow = CALCULATE_SHADOW_3D( P, light.position, light.range, light.shadowMapIndex );
	// else if (light.shadowType == SHADOW_TYPE_SOFT)
		// shadow = CALCULATE_SHADOW_3D_SOFT( P, light.position, light.range, light.shadowMapIndex );
		
	// shadow *= attenuation;
		
    return (diffuse + specular);
}

//----------------------------------------------------------------------
// SPOT LIGHT
//----------------------------------------------------------------------
float DoSpotCone( Light light, vec3 L )
{
    float minCos = cos( light.spotAngle );
    float maxCos = ( minCos + 1.0f ) / 2.0f;
    float cosAngle = dot( light.direction, -L );
    return smoothstep( minCos, maxCos, cosAngle ); 
}

vec4 DoSpotLight( Light light, vec3 V, vec3 P, vec3 N )
{
    vec3 L = (light.position - P);
    float dist = length( L );
    L = L / dist;
 
    float attenuation = DoAttenuation( light, dist );
    float spotIntensity = DoSpotCone( light, L );
 
    vec4 diffuse = DoDiffuse( light, L, N ) * attenuation * spotIntensity;
    vec4 specular = DoSpecular( light, V, L, N ) * attenuation * spotIntensity;
 
	// // Attenuation must be multiplied with the shadow in order to smoothly fade out the shadow
	// float shadow = 1.0;
	// if (light.shadowType == SHADOW_TYPE_HARD)
		// shadow = CALCULATE_SHADOW_2D( P, light.shadowMapIndex );
	// else if (light.shadowType == SHADOW_TYPE_SOFT)
		// shadow = CALCULATE_SHADOW_2D_SOFT( P, light.shadowMapIndex );
			
	// shadow *= attenuation;
			
    return (diffuse + specular);
}

//----------------------------------------------------------------------
// Calculates the lighting contribution for all lights in the light buffer.
// @Params:
//  "P": The position of the fragment in world space
//  "N": The normal of the fragment in world space
//----------------------------------------------------------------------
vec4 APPLY_LIGHTING( vec3 P, vec3 N )
{ 
	vec3 V = normalize( _Camera.pos - P ).xyz;
	vec3 normal = normalize( N );
	
	vec4 totalLight = { 0, 0, 0, 1 };

	for (int i = 0; i < _Lights.count; ++i)
	{
		Light light = _Lights.lights[i];
        switch( light.lightType )
        {
        case DIRECTIONAL_LIGHT:
            totalLight += DoDirectionalLight( light, V, P, normal );
            break;
		case POINT_LIGHT:
            totalLight += DoPointLight( light, V, P, normal );
            break;
		case SPOT_LIGHT:		
            totalLight += DoSpotLight( light, V, P, normal );
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
vec4 APPLY_LIGHTING( vec4 fragColor, vec3 P, vec3 N )
{ 
	vec4 lighting = APPLY_LIGHTING( P, N );		
	return fragColor * _Global._Ambient + fragColor * lighting; 
}