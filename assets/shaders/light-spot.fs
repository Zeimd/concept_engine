#version 330 core

#include "gbuffer-decode.fs"
#include "lighting-math.fs"

layout(location=0) out vec4 color;

uniform vec3 lightPos;
uniform vec3 lightDir;

vec3 lightColor = vec3(1.0,1.0,1.0);

uniform float coneCosOuter; // Cosine of the angle at spotlight edge

uniform float coneCosInner;

uniform float cutoffDistance; // Light has no effect after this distance

uniform float lightPower;

void main()
{
	vec3 f_eyePos = ReconstructPos();

	vec3 baseColor;
	vec3 normal;
	float metallic;
	float roughness;

	ReadGBuffer(baseColor,normal,metallic,roughness);

	roughness *= roughness;

	vec3 toLight = lightPos - f_eyePos;

	// Light attentuation 1/r^2

	float distSquared = dot(toLight,toLight);
	float inverseDist = inversesqrt(distSquared);

	// Clip at spotlight edge

	float coneDot = dot(lightDir,-normalize(toLight));

	if (coneDot < coneCosOuter) discard;

	// Adjust attentuation so that at maxDistance attentuation factor is zero

	inverseDist -= 1.0 / cutoffDistance;
	inverseDist = max(inverseDist,0);

	//inverseDist /= (4.0*pi); // Divide source energy evenly among sphere

	float powerTerm = inverseDist*lightPower;

	toLight = normalize(toLight);

	vec3 toEye = normalize(-f_eyePos);

	vec3 halfway = normalize(toLight+toEye);

	// Dot products for BRDF

	float lightDotNormal,eyeDotNormal,eyeDotHalf,halfDotNormal;
	float clip;

	DotProducts(normal,toEye,toLight,halfway,lightDotNormal,eyeDotNormal,eyeDotHalf,halfDotNormal,clip);

	float lightDotHalf = eyeDotHalf;

	float specularTerm = SpecularFactor(halfDotNormal,roughness);

	// Fresnel term

	float fresnel = FresnelTerm(eyeDotHalf,metallic);

	// Geometry term

	float geometry = GeometryTerm(lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,roughness);

	// Cook-Torrance model

	float div = 4.0*lightDotNormal*eyeDotNormal + epsilon;

	//float specularPower = lightDotNormal*specularTerm*fresnel*geometry / div;

	float specularPower = specularTerm*fresnel*geometry / div;
	//float specularPower = specularTerm*geometry / div;

	specularPower *= piDiv; // When real lighting units are used

	specularPower = clamp(specularPower,0,1);
	//specularPower = max(0,specularPower);

	specularPower *= powerTerm;

	vec3 specularColor = lightColor * specularPower;
	//vec3 specularColor = vec3(0,0,0);

	// Diffuse term

	// NOTE: powerTerm = diffuse/pi + specular/pi, so
	//       diffuse = pi*powerTerm - specular

	float diffusePower = max(0,pi*(powerTerm - specularPower)); // energy conservation
	diffusePower = diffusePower*lightDotNormal;

	vec3 diffuseColor = baseColor*diffusePower;

	vec3 finalColor = BlendColor(baseColor,diffuseColor,specularColor,metallic);

	/*
	float temp = sqrt(lightDotNormal*lightDotNormal);

	vec3 finalColor = vec3(0,temp,0);
	*/

/*
	vec3 finalColor = vec3(0,abs(normal.z),0);	
	if (normal.z < 0)
	{
		finalColor = vec3(0,0,abs(normal.z));	
	}
*/
	color = vec4(finalColor,1.0);
}
