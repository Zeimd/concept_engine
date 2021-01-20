#version 330 core

#include "gbuffer-decode.fs"
#include "lighting-math.fs"

layout(location=0) out vec4 color;

uniform vec3 lightDir;

uniform float lightPower;

vec3 lightColor = vec3(1.0,1.0,1.0);

void main()
{
	vec3 f_eyePos = ReconstructPos();

	vec3 baseColor;
	vec3 normal;
	float metallic;
	float roughness;

	ReadGBuffer(baseColor,normal,metallic,roughness);

	roughness *= roughness;

	//vec3 toLight = lightPos - f_eyePos;
	vec3 toLight = -lightDir;

	// Light attentuation 1/r^2

	/*
	float distSquared = dot(toLight,toLight);
	float inverseDist = inversesqrt(distSquared);

	// Adjust attentuation so that at maxDistance attentuation factor is zero

	inverseDist -= 1.0 / cutoffDistance;
	inverseDist = max(inverseDist,0);

	inverseDist /= (4.0*pi); // Divide source energy evenly among sphere
	*/

	float powerTerm = lightPower;

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

	float specularPower = specularTerm*fresnel*geometry / div;

	specularPower *= piDiv; // When real lighting units are used

	specularPower = clamp(specularPower,0,1);

	specularPower *= powerTerm;

	vec3 specularColor = lightColor * specularPower;

	// Diffuse term

	// NOTE: powerTerm = diffuse/pi + specular/pi, so
	//       diffuse = pi*powerTerm - specular

	float diffusePower = max(0,pi*(powerTerm - specularPower)); // energy conservation
	diffusePower = diffusePower*lightDotNormal;

	vec3 diffuseColor = baseColor*diffusePower;

	vec3 finalColor = BlendColor(baseColor,diffuseColor,specularColor,metallic);

	color = vec4(finalColor,1.0);
}
