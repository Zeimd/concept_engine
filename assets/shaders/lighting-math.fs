/*****************************************************************************
*
* lighting-math.fs
*
* Created By Jari Korkala 13/12/2015
*
* This file is not a valid fragment shader, but contains common
* declarations and functions.
*
*****************************************************************************/


float twopiDiv = 0.15915;
float piDiv = 0.31831;
float pi = 3.14159;

float epsilon = 0.00001; // Small value added to avoid division by zero

vec3 BaseReflectivity(vec3 baseColor,float metallic);

float GeometryImplicit(float lightDotNormal,float eyeDotNormal,float halfDotNormal,
	float lightDotHalf,float eyeDotHalf,float roughness)
{
	return lightDotNormal*eyeDotNormal;
}


float FactorGGX(float dotHalf,float dotNormal,float roughness)
{
	float alpha = roughness;

	float clipHalf = sign(dotHalf);
	float clipNormal = sign(dotNormal);

	float dotSquared = dotNormal * dotNormal;

	float root = sqrt(1+alpha*(1-dotSquared)/dotSquared);

	return 2.0*clipHalf*clipNormal/root;
}

float GeometrySmithGGX(float lightDotNormal,float eyeDotNormal,float halfDotNormal,
	float lightDotHalf,float eyeDotHalf,float roughness)
{
	return FactorGGX(lightDotHalf,lightDotNormal,roughness) *
				FactorGGX(eyeDotHalf,eyeDotNormal,roughness);
}


float GeometryTerm(float lightDotNormal,float eyeDotNormal,float halfDotNormal,
	float lightDotHalf,float eyeDotHalf,float roughness)
{
	return GeometrySmithGGX(lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,roughness);
	//return GeometryImplicit(lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,roughness);
}


void DotProducts(vec3 normal,vec3 toEye,vec3 toLight,vec3 halfway,
	out float lightDotNormal,out float eyeDotNormal,out float eyeDotHalf,out float halfDotNormal,
	out float clip)
{
	eyeDotNormal = dot(toEye,normal);
	
	//if (eyeDotNormal <= 0.0) discard;

	eyeDotNormal = max(0,eyeDotNormal);

	lightDotNormal = dot(toLight,normal);

	//if (lightDotNormal <= 0.0) discard;

	lightDotNormal = max(0,lightDotNormal);

	clip = sign(lightDotNormal);

	eyeDotHalf = max(0,dot(toEye,halfway));

	halfDotNormal = max(0,dot(halfway,normal));
}


float SpecularPhong(float halfDotNormal,float roughness)
{	
	float alpha = roughness*roughness;

	float scaling = 2*(1.0/(alpha*alpha)-1.0);

	float clip = step(0.0,halfDotNormal);

	float specular = clip*(scaling+2.0)*twopiDiv*pow(halfDotNormal,scaling);

	return specular;
}

float SpecularGGX(float halfDotNormal,float roughness)
{
	float chi = step(0,halfDotNormal);

	float alpha = roughness;//*roughness;
	//float alpha = pow((0.3+roughness*0.7),6);

	alpha *= alpha;

	float factor = 1+halfDotNormal*halfDotNormal*(alpha-1);

	//float div = pi*factor*factor;

	// Note: energy conservation requires division by PI here,
	// but defer that to main shader function as optional
	float div = factor*factor;

	div = max(div,0.00001);

	float result = chi*alpha/div; 
	//float result = alpha/div; 
	return result;
}

float SpecularFactor(float halfDotNormal,float roughness)
{
	//return SpecularPhong(halfDotNormal,roughness);
	return SpecularGGX(halfDotNormal,roughness);
}

float MaterialMix(float metallic)
{
	return smoothstep(0.2,0.45,metallic);
}

vec3 FresnelTerm(vec3 baseColor,float metallic,float eyeDotHalf)
{
	vec3 factor = BaseReflectivity(baseColor,metallic);

	// Schlick's approximation

	return factor+(vec3(1.0,1.0,1.0)-factor)*pow(1-eyeDotHalf,5.0);
}

vec3 DiffuseColor(vec3 baseColor,float metallic)
{
	return mix(baseColor,vec3(0,0,0),MaterialMix(metallic));
}

vec3 BaseReflectivity(vec3 baseColor,float metallic)
{
	// Semiconductors have metallic value in the range [0.2,0.45]
	// If MetallicTerm() < 0.2, assume dielectric
	float semiconductorMix = MaterialMix(metallic);

	// Use value stored in texture for dielectrics
	vec3 f0 = vec3(metallic,metallic,metallic);

	return mix(f0,baseColor,semiconductorMix);
}


vec3 CookTorrance(vec3 baseColor,float lightDotNormal,float eyeDotNormal,float halfDotNormal,
	float lightDotHalf,float eyeDotHalf,float metallic,float roughness)
{
	float div = 4.0*lightDotNormal*eyeDotNormal + epsilon;

	float specularDistribution = SpecularFactor(halfDotNormal,roughness);

	float geometry = GeometryTerm(lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,roughness);

	float specularFactor = specularDistribution * geometry / div;

	specularFactor *= piDiv; // When real lighting units are used

	specularFactor = clamp(specularFactor,0,1);

	return specularFactor * FresnelTerm(baseColor,metallic,eyeDotHalf);
}

/**
 * Select correct lighting for dielectric, semiconductor, or conductor (metal)
 * material.
 */
vec3 BlendColor(vec3 surfaceColor,vec3 diffuseColor,vec3 specularColor,float metallic)
{
	// Semiconductors have metallic value in the range [0.2,0.45]
	// If MetallicTerm() < 0.2, assume dielectric
	float semiconductorMix = smoothstep(0.2,0.45,metallic);

	vec3 dielectricColor = diffuseColor + specularColor;
	vec3 metalColor = surfaceColor * specularColor;

	return mix(dielectricColor,metalColor,semiconductorMix);
}


#if defined(LIGHTMAP_SCALAR)
	void LightmapContribution(vec3 color,vec3 normal,vec2 uv,float metallic,float roughness)
	{
		//float lightDotNormal,eyeDotNormal,eyeDotHalf,halfDotNormal;
		//float clip;

		// With a directionless lightmap can't do better than this
		/*	
		vec3 toLight = normal;
		vec3 toEye = -f_position;

		DotProducts(normal,toEye,toLight,halfway,lightDotNormal,eyeDotNormal,eyeDotHalf,halfDotNormal,clip);

		float lightDotHalf = eyeDotHalf;

		vec3 specularPower = CookTorrance(color.xyz,lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,metallic,roughness);

		vec3 diffusePower = max(vec3(0,0,0),pi*(vec3(1,1,1) - specularPower)); // energy conservation

		vec3 diffuseColor = DiffuseColor(color.xyz,metallic);

		diffusePower *= diffuseColor;
		*/

		//vec3 lightColor = texture(lightMap,uv).xyz;
		vec3 lightColor = texture(lightMap,vec2(uv.x,uv.y)).xyz;

		vec3 finalColor = color * lightColor;
		//vec3 finalColor = lightColor;
		//vec3 finalColor = color;

		out_lightAccumulator = vec4(finalColor,1.0);
	}
#else
	void LightmapContribution(vec3 color,vec3 normal,vec2 uv,float metallic,float roughness)
	{
	}
#endif
