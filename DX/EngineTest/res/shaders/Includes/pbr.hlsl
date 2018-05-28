
#define DIELECTRIC_IOR 0.04

TextureCube<float4> diffuseIrradianceMap;
SamplerState samplerDiffuseIrradianceMap; 

TextureCube<float4> specularReflectionMap;
SamplerState samplerSpecularReflectionMap;

Texture2D brdfLUT;
SamplerState samplerbrdfLUT;

//----------------------------------------------------------------------
// LIGHTING - PBR
//----------------------------------------------------------------------

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
float3 CalcLight( Light light, float3 radiance, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic )
{	
	float3 F0 = float3( DIELECTRIC_IOR, DIELECTRIC_IOR, DIELECTRIC_IOR ); 
	F0        = lerp( F0, albedo, metallic );
	float3 F  = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
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
	
    float3 F0 = float3( DIELECTRIC_IOR, DIELECTRIC_IOR, DIELECTRIC_IOR ); 
	F0        = lerp( F0, albedo, metallic );
	float3 F  = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
	float3 kS = F;
	float3 kD = float3( 1,1,1 ) - kS;
  
	kD *= 1.0 - metallic;	

	float3 L = normalize( -light.direction );
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
// POINT LIGHT
//----------------------------------------------------------------------
float3 DoPointLight( Light light, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic  )
{  
    float distance  = length( light.position - P );
    float3 radiance = light.color.rgb * DoAttenuation( light, distance ) * light.intensity; 
 
    return CalcLight( light, radiance, albedo, V, P, N, roughness, metallic );
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

float3 DoSpotLight( Light light, float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic )
{    
	float3 L = (light.position - P);
	float distance  = length( light.position - P );
    float3 radiance = light.color.rgb * DoAttenuation( light, distance ) * light.intensity * DoSpotCone( light, L ); 
 
    return CalcLight( light, radiance, albedo, V, P, N, roughness, metallic );
}

//----------------------------------------------------------------------
// IMAGE BASED LIGHTING
//----------------------------------------------------------------------
float3 getIBL( float3 albedo, float3 V, float3 P, float3 N, float roughness, float metallic, float maxReflectionLOD )
{
	float3 R = reflect( -V, N ); 
		
	float3 F0 = float3( DIELECTRIC_IOR, DIELECTRIC_IOR, DIELECTRIC_IOR ); 
	F0        = lerp( F0, albedo, metallic );
	float3 F  = fresnelSchlickRoughness( max( dot( N, V ), 0.0 ), F0, roughness );  
	
	float3 kS = F;
	float3 kD = float3( 1.0, 1.0, 1.0 ) - kS;  
	kD *= 1.0 - metallic;	
	
	float3 irradiance = TO_LINEAR( diffuseIrradianceMap.Sample( samplerDiffuseIrradianceMap, N ).rgb );
	float3 diffuse    = irradiance * albedo;	
		
    float3 prefilteredColor = TO_LINEAR( specularReflectionMap.SampleLevel( samplerSpecularReflectionMap, R,  roughness * maxReflectionLOD ).rgb );    
    float2 brdf = brdfLUT.Sample( samplerbrdfLUT, float2( max( dot( N, V ), 0.0 ), roughness ) ).rg;
    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	
	float3 ibl = (kD * diffuse) + specular; 
	
	return ibl;
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
float4 APPLY_LIGHTING( float4 fragColor, float3 P, float3 normal, float roughness, float metallic, float maxReflectionLOD )
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
	
	float3 lighting = fragColor.rgb * Lo;	
	
	float3 ibl = getIBL( fragColor.rgb, V, P, N, roughness, metallic, maxReflectionLOD );
	
	float3 result = ibl + lighting;
	
	// Reinhard tonemapping
	result = result / (result + float3(1,1,1));
	
	// Gamma correct
	result = TO_SRGB( result );	
	
	return float4( result, 1 ); 
}