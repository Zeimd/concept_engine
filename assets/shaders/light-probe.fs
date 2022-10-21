#version 330 core

#include "gbuffer-decode.fs"
#include "lighting-math.fs"

uniform mat4 cameraReverseRotation;

uniform samplerCube reflectionEnv;
uniform samplerCube diffuseEnv;

uniform float maxEnvLOD;

#if defined(ENVMAP_PARALLAX_CUBE)

// Axis-aligned bounding cube used for parallax correction

uniform vec3 cameraPos;

uniform vec3 cubeCenterWorldPos;

uniform float cubeSideHalf;

#endif

#if defined(ENVMAP_PARALLAX_BOX)

// Arbitrary axis-aligned bounding box used for parallax correction

// TODO

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
	vec3 specularEnv = textureLod(reflectionEnv,cubeDir,lod).rgb;

	vec3 diffuseColor = DiffuseColor(baseColor,metallic);

	//diffuseColor *= 0.005;

	//vec3 finalColor = specularEnv;

	vec3 finalColor = diffuseColor * diffuseEnv + specularFactor * specularEnv;

	//vec3 finalColor = diffuseColor;

	color = vec4(finalColor,1.0);
}

// returns vec3(k, u, v) : if k <= 0, the ray isn't moving towards the face and there is no intersection
vec4 RayCubeFaceCollision(vec3 rayStart, vec3 rayDir, vec3 faceCenter, vec3 faceNormal, float sideHalf, vec3 baseU, vec3 baseV)
{
	float rayDotNormal = dot(rayDir, faceNormal);

	if (rayDotNormal >= 0.0)
	{
		return vec4(-1.0, vec3(0.0,0.0,0.0));
	}

	float normalDotDiff = dot(faceNormal, faceCenter - rayStart);

	float k = normalDotDiff / rayDotNormal;

	if (k <= 0.0)
	{
		return vec4(-1.0, vec3(0.0,0.0,0.0));
	}

	vec3 intersect = rayStart + k*rayDir;

	vec3 planar = intersect - faceCenter;

	float u = dot(planar, baseU);
	float v = dot(planar, baseV);

	if (abs(u) > sideHalf || abs(v) > sideHalf)
	{
		return vec4(-1.0, vec3(0.0,0.0,0.0));
	}

	return vec4(k, intersect);
}


#if defined(ENVMAP_PARALLAX_CUBE)

	vec3 ParallaxCorrection(vec3 f_eyePos, vec3 reflectDir_world)
	{
		vec3 f_worldPos = (cameraReverseRotation * vec4(f_eyePos,1.0)).xyz + cameraPos;

		vec3 f_boxPos = f_worldPos - cubeCenterWorldPos;

		vec3 faceCenter;
		vec3 faceNormal;

		vec3 baseU;
		vec3 baseV;

		// +X face

		faceCenter = vec3(cubeSideHalf, 0.0, 0.0);
		faceNormal = vec3(-1.0, 0.0, 0.0);

		baseU = vec3(0.0, 1.0, 0.0);
		baseV = vec3(0.0, 0.0, 1.0);

		vec4 resultXP = RayCubeFaceCollision(f_boxPos, reflectDir_world, faceCenter, faceNormal, cubeSideHalf, baseU, baseV);

		if (resultXP.x > 0)
		{
			return resultXP.yzw;
		}

		// -X face

		faceCenter = vec3(-cubeSideHalf, 0.0, 0.0);
		faceNormal = vec3(1.0, 0.0, 0.0);

		baseU = vec3(0.0, 1.0, 0.0);
		baseV = vec3(0.0, 0.0, -1.0);

		vec4 resultXN = RayCubeFaceCollision(f_boxPos, reflectDir_world, faceCenter, faceNormal, cubeSideHalf, baseU, baseV);

		if (resultXN.x > 0)
		{
			return resultXN.yzw;
		}

		// +Y face

		faceCenter = vec3(0.0, cubeSideHalf, 0.0);
		faceNormal = vec3(0.0, -1.0, 0.0);

		baseU = vec3(1.0, 0.0, 0.0);
		baseV = vec3(0.0, 0.0, -1.0);

		vec4 resultYP = RayCubeFaceCollision(f_boxPos, reflectDir_world, faceCenter, faceNormal, cubeSideHalf, baseU, baseV);

		if (resultYP.x > 0)
		{
			return resultYP.yzw;
		}

		// -Y face

		faceCenter = vec3(0.0, -cubeSideHalf, 0.0);
		faceNormal = vec3(0.0, 1.0, 0.0);

		baseU = vec3(1.0, 0.0, 0.0);
		baseV = vec3(0.0, 0.0, 1.0);

		vec4 resultYN = RayCubeFaceCollision(f_boxPos, reflectDir_world, faceCenter, faceNormal, cubeSideHalf, baseU, baseV);

		if (resultYN.x > 0)
		{
			return resultYN.yzw;
		}

		// +Z face

		faceCenter = vec3(0.0, 0.0, cubeSideHalf);
		faceNormal = vec3(0.0, 0.0, -1.0);

		baseU = vec3(-1.0, 0.0, 0.0);
		baseV = vec3(0.0, 1.0, 0.0);

		vec4 resultZP = RayCubeFaceCollision(f_boxPos, reflectDir_world, faceCenter, faceNormal, cubeSideHalf, baseU, baseV);

		if (resultZP.x > 0)
		{
			return resultZP.yzw;
		}

		// -Z face

		faceCenter = vec3(0.0, 0.0, -cubeSideHalf);
		faceNormal = vec3(0.0, 0.0, 1.0);

		baseU = vec3(1.0, 0.0, 0.0);
		baseV = vec3(0.0, 1.0, 0.0);

		vec4 resultZN = RayCubeFaceCollision(f_boxPos, reflectDir_world, faceCenter, faceNormal, cubeSideHalf, baseU, baseV);

		if (resultZN.x > 0)
		{
			return resultZN.yzw;
		}

		// Ray didn't hit any face, so no lighting from this environment map
		discard;
	}

#elif defined(ENVMAP_PARALLAX_BOX)

	vec3 ParallaxCorrection(vec3 f_worldPos, vec3 reflectDir_world)
	{

	}

#else

	vec3 ParallaxCorrection(vec3 f_worldPos, vec3 reflectDir_world)
	{
		return reflectDir_world;
	}

#endif