#version 330 core

#include "gbuffer-decode.fs"
#include "lighting-math.fs"

layout(location=0) out vec4 color;

uniform sampler2D shadowMap;

uniform vec3 lightPos;

uniform vec3 lightDir;

uniform mat4 lightProjection;

uniform mat4 cameraTransform;

vec3 lightColor = vec3(1.0,1.0,1.0);

uniform float coneCosOuter; // Cosine of the angle at spotlight edge

uniform float coneCosInner;

uniform float cutoffDistance; // Light has no effect after this distance

uniform float lightPower;

uniform float depthBias;

float SampleShadow(vec3 position,float lightDotNormal)
{
	vec4 projected = lightProjection * vec4(position,1.0);

	vec4 ndc = projected / projected.w;

	// Remap to [0,1] range for texture fetch and depth value comparison
	vec3 texCoord = 0.5 * ndc.xyz + vec3(0.5,0.5,0.5);

	float result = 1.0;

	if ( texture( shadowMap , texCoord.xy).r + depthBias < texCoord.z)
	{
		discard;
		//result = 0.0;
	}

	return result;
}

void main()
{
	vec3 f_eyePos = ReconstructPos();

	vec3 baseColor;
	vec3 normal;
	float metallic;
	float roughness;

	ReadGBuffer(baseColor,normal,metallic,roughness);

	vec3 toLight = lightPos - f_eyePos;

	// Clip at spotlight edge

	float coneDot = dot(lightDir,-normalize(toLight));
	if (coneDot < coneCosOuter) discard;

	float shadowing = SampleShadow(f_eyePos,max(0,dot(toLight,normal)));

	roughness *= roughness;

	// Light attentuation 1/r^2

	float distSquared = dot(toLight,toLight);
	float inverseDist = inversesqrt(distSquared);

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

	vec3 specularPower = CookTorrance(baseColor,lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,metallic,roughness);

	// Diffuse term

	// NOTE: powerTerm = diffuse/pi + specular/pi, so
	//       diffuse = pi*powerTerm - specular

	vec3 diffusePower = max(vec3(0,0,0),pi*(vec3(1,1,1) - specularPower)); // energy conservation
	diffusePower = diffusePower*lightDotNormal;

	vec3 diffuseColor = DiffuseColor(baseColor,metallic);

	diffusePower *= diffuseColor;

	vec3 finalColor = powerTerm * (diffusePower + specularPower) * lightColor;// * lightDotNormal;

	color = vec4(finalColor,1.0);
}
