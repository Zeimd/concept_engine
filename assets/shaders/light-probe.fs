#version 330 core

#include "gbuffer-decode.fs"
#include "lighting-math.fs"

uniform mat4 cameraReverseRotation;

uniform samplerCube reflectionEnv;
uniform samplerCube diffuseEnv;

uniform float maxEnvLOD;

/*
// DEPRECATED: subcase of ENVMAP_PARALLAX_AA_BOX
#if defined(ENVMAP_PARALLAX_AA_CUBE)

// Axis-aligned bounding cube used for parallax correction

#define ENVMAP_PARALLAX_CORRECTION

uniform float cubeSideHalf;
*/

#if defined(ENVMAP_PARALLAX_AA_BOX)

#define ENVMAP_PARALLAX_CORRECTION

// Arbitrary axis-aligned bounding box used for parallax correction

uniform vec3 boxSideHalf;

#endif

// Common parallax correction variables
#if defined(ENVMAP_PARALLAX_CORRECTION)

uniform vec3 cameraPos;

uniform vec3 world_cubeGenPos;

#endif


vec3 ParallaxCorrection(vec3 f_worldPos, vec3 reflectDir_world);

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

	vec3 rayDir_eye = normalize(f_eyePos);

	vec3 reflectDir_eye = reflect(rayDir_eye,normal);

	// Transform reflected direction to world space (where the cube map is defined)

	vec3 reflectDir_world = (cameraReverseRotation * vec4(reflectDir_eye,1.0)).xyz;

	vec3 cubeDir = ParallaxCorrection(f_eyePos, reflectDir_world);
	//vec3 cubeDir = reflectDir_world;

	float lod = maxEnvLOD*roughness;

	// Direction vectors point away from the surface
	vec3 toLight = reflectDir_eye;
	vec3 toEye = -rayDir_eye;

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
	vec3 diffuseColor = DiffuseColor(baseColor,metallic);
	//vec3 diffuseColor = vec3(0.0,0.0,0.0);
	
	vec3 specularEnv = textureLod(reflectionEnv,cubeDir,lod).rgb;
	//vec3 specularEnv = vec3(0.0,0.0,0.0);

	vec3 finalColor = diffuseColor * diffuseEnv + specularFactor * specularEnv;

	color = vec4(finalColor,1.0);
}

// Calculate point of collision between ray and a box face.
// returns vec3(k, u, v) : if k <= 0, the ray isn't moving towards the face and there is no intersection
vec4 RayFaceCollision(vec3 rayStart, vec3 rayDir, vec3 faceCenter, vec3 faceNormal, vec3 baseU, vec3 baseV, float uLimit, float vLimit)
{
	float rayDotNormal = dot(rayDir, faceNormal);

	// We want collisions against the inner surface of a box, so
	// ignore the face if the ray is coming from outside of the box

	if (rayDotNormal >= 0.0)
	{
		return vec4(-1.0, vec3(0.0,0.0,0.0));
	}

	// Use the plane equation to calculate the distance from ray's starting point to the face

	float normalDotDiff = dot(faceNormal, faceCenter - rayStart);

	float k = normalDotDiff / rayDotNormal;

	// Ignore face if the reflected ray would have to travel backwards to reach it
	if (k <= 0.0)
	{
		return vec4(-1.0, vec3(0.0,0.0,0.0));
	}

	vec3 intersect = rayStart + k*rayDir;

	vec3 planar = intersect - faceCenter;

	float u = dot(planar, baseU);
	float v = dot(planar, baseV);

	// Ignore the face if the ray hits the plane outside of the face's circumference
	if (abs(u) > uLimit || abs(v) > vLimit)
	{
		return vec4(-1.0, vec3(0.0,0.0,0.0));
	}

	return vec4(k, intersect);
}

#if defined(ENVMAP_PARALLAX_AA_BOX)

	// Calculate parallax correction by determining which inner face of the box the reflected ray hits
	// Because of this the face normals point inwards
	vec3 ParallaxCorrection(vec3 f_eyePos, vec3 reflectDir_world)
	{
		vec3 f_worldPos = (cameraReverseRotation * vec4(f_eyePos,1.0)).xyz + cameraPos;

		vec3 f_boxPos = f_worldPos - world_cubeGenPos;

		vec3 faceCenter;
		vec3 faceNormal;

		vec3 baseU;
		vec3 baseV;

		// +X face

		faceCenter = vec3(boxSideHalf.x, 0.0, 0.0);
		faceNormal = vec3(-1.0, 0.0, 0.0);

		baseU = vec3(0.0, 1.0, 0.0);
		baseV = vec3(0.0, 0.0, 1.0);

		vec4 resultXP = RayFaceCollision(f_boxPos, reflectDir_world, faceCenter, 
			faceNormal, baseU, baseV, boxSideHalf.y, boxSideHalf.z);

		if (resultXP.x > 0)
		{
			return resultXP.yzw;
		}

		// -X face

		faceCenter = vec3(-boxSideHalf.x, 0.0, 0.0);
		faceNormal = vec3(1.0, 0.0, 0.0);

		baseU = vec3(0.0, 1.0, 0.0);
		baseV = vec3(0.0, 0.0, -1.0);

		vec4 resultXN = RayFaceCollision(f_boxPos, reflectDir_world, faceCenter, 
			faceNormal, baseU, baseV, boxSideHalf.y, boxSideHalf.z);

		if (resultXN.x > 0)
		{
			return resultXN.yzw;
		}

		// +Y face

		faceCenter = vec3(0.0, boxSideHalf.y, 0.0);
		faceNormal = vec3(0.0, -1.0, 0.0);

		baseU = vec3(1.0, 0.0, 0.0);
		baseV = vec3(0.0, 0.0, -1.0);

		vec4 resultYP = RayFaceCollision(f_boxPos, reflectDir_world, faceCenter, 
			faceNormal, baseU, baseV, boxSideHalf.x, boxSideHalf.z);

		if (resultYP.x > 0)
		{
			return resultYP.yzw;
		}

		// -Y face

		faceCenter = vec3(0.0, -boxSideHalf.y, 0.0);
		faceNormal = vec3(0.0, 1.0, 0.0);

		baseU = vec3(1.0, 0.0, 0.0);
		baseV = vec3(0.0, 0.0, 1.0);

		vec4 resultYN = RayFaceCollision(f_boxPos, reflectDir_world, faceCenter, 
			faceNormal, baseU, baseV, boxSideHalf.x, boxSideHalf.z);

		if (resultYN.x > 0)
		{
			return resultYN.yzw;
		}

		// +Z face

		faceCenter = vec3(0.0, 0.0, boxSideHalf.z);
		faceNormal = vec3(0.0, 0.0, -1.0);

		baseU = vec3(-1.0, 0.0, 0.0);
		baseV = vec3(0.0, 1.0, 0.0);

		vec4 resultZP = RayFaceCollision(f_boxPos, reflectDir_world, faceCenter, 
			faceNormal, baseU, baseV, boxSideHalf.x, boxSideHalf.y);

		if (resultZP.x > 0)
		{
			return resultZP.yzw;
		}

		// -Z face

		faceCenter = vec3(0.0, 0.0, -boxSideHalf.z);
		faceNormal = vec3(0.0, 0.0, 1.0);

		baseU = vec3(1.0, 0.0, 0.0);
		baseV = vec3(0.0, 1.0, 0.0);

		vec4 resultZN = RayFaceCollision(f_boxPos, reflectDir_world, faceCenter, 
			faceNormal, baseU, baseV, boxSideHalf.x, boxSideHalf.y);

		if (resultZN.x > 0)
		{
			return resultZN.yzw;
		}

		// Ray didn't hit any face, so no lighting from this environment map
		discard;

	}

#else

	vec3 ParallaxCorrection(vec3 f_worldPos, vec3 reflectDir_world)
	{
		return reflectDir_world;
	}

#endif