#version 330 core

#include "gbuffer-decode.fs"
#include "lighting-math.fs"

uniform mat4 cameraReverseRotation;

uniform samplerCube reflectionEnv;
uniform samplerCube diffuseEnv;

uniform float maxEnvLOD;

layout(location=0) out vec4 color;

void main()
{
	vec3 f_eyePos = ReconstructPos();

	vec3 baseColor;
	vec3 normal;
	float metallic;
	float linearRoughness;

	ReadGBuffer(baseColor,normal,metallic,linearRoughness);

	float roughness = linearRoughness*linearRoughness;

	// raydir, reflectDir in eye space

	vec3 rayDir = normalize(f_eyePos);

	vec3 reflectDir = reflect(rayDir,normal);

	// Transform reflected direction to world space (where the cube map is defined)

	vec3 cubeDir = (cameraReverseRotation * vec4(reflectDir,1.0)).xyz;

	float lod = maxEnvLOD*roughness;

	// Direction vectors point away from the surface
	vec3 toLight = reflectDir;
	vec3 toEye = -rayDir;

	vec3 halfway = normalize(toLight+toEye);

	// Dot products for BRDF

	float lightDotNormal,eyeDotNormal,eyeDotHalf,halfDotNormal;
	float clip;

	DotProducts(normal,toEye,toLight,halfway,lightDotNormal,eyeDotNormal,eyeDotHalf,halfDotNormal,clip);

	float lightDotHalf = eyeDotHalf;

	vec3 specularFactor = CookTorrance(baseColor,lightDotNormal,eyeDotNormal,halfDotNormal,lightDotHalf,eyeDotHalf,metallic,roughness);

	//vec3 diffusePower = max(vec3(0,0,0),pi*(vec3(1,1,1) - specularPower)); // energy conservation
	//diffusePower = diffusePower*lightDotNormal;

	// Diffuse term

	vec3 eyeNormal = (cameraReverseRotation * vec4(normal,1.0)).xyz;

	vec3 diffuseEnv = textureLod(diffuseEnv,eyeNormal,0).rgb;
	vec3 specularEnv = textureLod(reflectionEnv,cubeDir,lod).rgb;

	vec3 diffuseColor = DiffuseColor(baseColor,metallic);

	//diffuseColor *= 0.005;

	//vec3 finalColor = specularEnv;

	vec3 finalColor = diffuseColor * diffuseEnv + specularFactor * specularEnv;

	//vec3 finalColor = diffuseColor;

	color = vec4(finalColor,1.0);
}

