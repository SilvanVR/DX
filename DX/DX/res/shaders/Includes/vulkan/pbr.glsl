
#define DIELECTRIC_IOR 0.04

layout (set = 2, binding = 0) uniform SHADER
{
	float maxReflectionLOD;
};
layout (set = 2, binding = 1) uniform sampler2D brdfLUT;
layout (set = 2, binding = 2) uniform samplerCube diffuseIrradianceMap;
layout (set = 2, binding = 3) uniform samplerCube specularReflectionMap;

//----------------------------------------------------------------------
// LIGHTING - PBR
//----------------------------------------------------------------------

//----------------------------------------------------------------------
float DoAttenuation( Light light, float d )
{
    return max( 1 - smoothstep(0, light.range, d), 0 );
}

//----------------------------------------------------------------------
vec3 fresnelSchlickRoughness( float cosTheta, vec3 F0, float roughness )
{
	float oneMinusRoughness = 1.0 - roughness;
    return F0 + ( max( vec3(oneMinusRoughness, oneMinusRoughness, oneMinusRoughness), F0 ) - F0) * pow( 1.0 - cosTheta, 5.0 );
}

//----------------------------------------------------------------------
float DistributionGGX( vec3 N, vec3 H, float roughness )
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
float GeometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
{
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotL = max( dot( N, L ), 0.0 );
    float ggx2  = GeometrySchlickGGX( NdotV, roughness );
    float ggx1  = GeometrySchlickGGX( NdotL, roughness );
	
    return ggx1 * ggx2;
}

//----------------------------------------------------------------------
vec3 CalcLight( Light light, vec3 radiance, vec3 albedo, vec3 V, vec3 P, vec3 N, float roughness, float metallic )
{	
	vec3 F0 = vec3( DIELECTRIC_IOR, DIELECTRIC_IOR, DIELECTRIC_IOR ); 
	F0      = mix( F0, albedo, metallic );
	vec3 F  = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
	vec3 kS = F;
	vec3 kD = vec3( 1,1,1 ) - kS;
  
	kD *= 1.0 - metallic;	

	vec3 L = normalize( light.position - P );
    vec3 H = normalize( V + L );
		
	float NDF = DistributionGGX( N, H, roughness );       
	float G   = GeometrySmith( N, V, L, roughness ); 
		
	vec3 nominator  = NDF * G * F;
	float denominator = 4 * max( dot( V, N ), 0.0) * max( dot( L, N ), 0.0) + 0.001; 
	vec3 specular   = nominator / denominator;  
  
    float NdotL = max( dot( N, L ), 0.0 );        
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	return Lo;  
}

//----------------------------------------------------------------------
// DIRECTIONAL LIGHT
//----------------------------------------------------------------------
vec3 DoDirectionalLight( Light light, vec3 albedo, vec3 V, vec3 P, vec3 N, float roughness, float metallic )
{
    vec3 radiance = light.color.rgb * light.intensity;
	
    vec3 F0 = vec3( DIELECTRIC_IOR, DIELECTRIC_IOR, DIELECTRIC_IOR ); 
	F0      = mix( F0, albedo, metallic );
	vec3 F  = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
	vec3 kS = F;
	vec3 kD = vec3( 1,1,1 ) - kS;
  
	kD *= 1.0 - metallic;	

	vec3 L = normalize( -light.direction );
    vec3 H = normalize( V + L );
		
	float NDF = DistributionGGX( N, H, roughness );       
	float G   = GeometrySmith( N, V, L, roughness ); 
		
	vec3 nominator  = NDF * G * F;
	float denominator = 4 * max( dot( V, N ), 0.0) * max( dot( L, N ), 0.0) + 0.001; 
	vec3 specular   = nominator / denominator;  
  
    float NdotL = max( dot( N, L ), 0.0 );        
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	float shadow = 1.0;
	if (light.shadowType == SHADOW_TYPE_HARD)
		shadow = CALCULATE_SHADOW_DIR( P, light.range, light.shadowMapIndex );
	else if (light.shadowType == SHADOW_TYPE_SOFT)
		shadow = CALCULATE_SHADOW_DIR_SOFT( P, light.range, light.shadowMapIndex );
	else if (light.shadowType == SHADOW_TYPE_CSM)
		shadow = CALCULATE_SHADOW_CSM( P, light.shadowMapIndex );
	else if (light.shadowType == SHADOW_TYPE_CSM_SOFT)
		shadow = CALCULATE_SHADOW_CSM_SOFT( P, light.shadowMapIndex );
	
	return Lo * shadow;  
}

//----------------------------------------------------------------------
// POINT LIGHT
//----------------------------------------------------------------------
vec3 DoPointLight( Light light, vec3 albedo, vec3 V, vec3 P, vec3 N, float roughness, float metallic  )
{  
    float distance  = length( light.position - P );
	float attenuation = DoAttenuation( light, distance );
    vec3 radiance = light.color.rgb * attenuation * light.intensity; 
 
  	float shadow = 1.0;
	if (light.shadowType == SHADOW_TYPE_HARD)
		shadow = CALCULATE_SHADOW_3D( P, light.position, light.range, light.shadowMapIndex );
	else if (light.shadowType == SHADOW_TYPE_SOFT)
		shadow = CALCULATE_SHADOW_3D_SOFT( P, light.position, light.range, light.shadowMapIndex );
		
	shadow *= attenuation;
 
    return CalcLight( light, radiance, albedo, V, P, N, roughness, metallic ) * shadow;
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

vec3 DoSpotLight( Light light, vec3 albedo, vec3 V, vec3 P, vec3 N, float roughness, float metallic )
{    
	vec3 L = (light.position - P);
	float distance  = length( light.position - P );
	float attenuation = DoAttenuation( light, distance );
    vec3 radiance = light.color.rgb * attenuation * light.intensity * DoSpotCone( light, L ); 
 
  	// Attenuation must be multiplied with the shadow in order to smoothly fade out the shadow
	float shadow = 1.0;
	if (light.shadowType == SHADOW_TYPE_HARD)
		shadow = CALCULATE_SHADOW_2D( P, light.shadowMapIndex );
	else if (light.shadowType == SHADOW_TYPE_SOFT)
		shadow = CALCULATE_SHADOW_2D_SOFT( P, light.shadowMapIndex );
			
	shadow *= attenuation;
 
    return CalcLight( light, radiance, albedo, V, P, N, roughness, metallic ) * shadow;
}

//----------------------------------------------------------------------
// IMAGE BASED LIGHTING
//----------------------------------------------------------------------
vec3 getIBL( vec3 albedo, vec3 V, vec3 P, vec3 N, float roughness, float metallic )
{
	vec3 R = reflect( -V, N ); 
		
	vec3 F0 = vec3( DIELECTRIC_IOR, DIELECTRIC_IOR, DIELECTRIC_IOR ); 
	F0      = mix( F0, albedo, metallic );
	vec3 F  = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
	vec3 kS = F;
	vec3 kD = vec3( 1.0, 1.0, 1.0 ) - kS;  
	kD *= 1.0 - metallic;	
	
	vec3 irradiance = texture( diffuseIrradianceMap, N ).rgb;
	vec3 diffuse    = irradiance * albedo;	
		
    vec3 prefilteredColor = textureLod( specularReflectionMap, R,  roughness * maxReflectionLOD ).rgb;    
    vec2 brdf = texture( brdfLUT, vec2( max( dot( N, V ), 0.0 ), roughness ) ).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	
	vec3 ibl = (kD * diffuse) + specular; 
	
	return ibl;
}

//----------------------------------------------------------------------
// Applies lighting to a given fragment.
// @Params:
//  "fragColor": The color of the fragment which will receive the lighting in LINEAR SPACE
//  "P": The position of the fragment in world space
//  "N": The normal of the fragment in world space
//  "roughness": The roughness of the fragment
//  "metallic": Metallic of the fragment
//----------------------------------------------------------------------
vec4 APPLY_LIGHTING( vec4 fragColor, vec3 P, vec3 normal, float roughness, float metallic )
{ 
	vec3 V = normalize( _Camera.pos - P ).xyz;
	vec3 N = normalize( normal );
	
	vec3 Lo = { 0, 0, 0 };

	for (int i = 0; i < _Lights.count; i++)
	{
        switch( _Lights.lights[i].lightType )
        {
        case DIRECTIONAL_LIGHT:
            Lo += DoDirectionalLight( _Lights.lights[i], fragColor.rgb, V, P, N, roughness, metallic );
            break;
		case POINT_LIGHT:		
            Lo += DoPointLight( _Lights.lights[i], fragColor.rgb, V, P, N, roughness, metallic );
            break;
		case SPOT_LIGHT:
			Lo += DoSpotLight( _Lights.lights[i], fragColor.rgb, V, P, N, roughness, metallic );
            break;
        }
	}
	
	vec3 lighting = fragColor.rgb * Lo;	
	
	vec3 ibl = getIBL( fragColor.rgb, V, P, N, roughness, metallic );
	
	vec3 result = _Global._Ambient * ibl + lighting;
		
	return vec4( result, fragColor.a ); 
}