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


vec4 ParallaxCorrection(vec3 f_worldPos, vec3 reflectDir_world);

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

    vec4 cubeDir = ParallaxCorrection(f_eyePos, reflectDir_world);
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
    
    vec3 specularEnv = textureLod(reflectionEnv,cubeDir.xyz,lod).rgb * cubeDir.w;
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

    // Ignore face if the reflected ray would have to travel backwards to reach it. Also ignore
    // case where reflected ray hits the surface it reflects from (k == 0)
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

    // New version based on article
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
    vec4 ParallaxCorrectionNew(vec3 f_eyePos, vec3 reflectDir_world)
    {
        vec3 f_worldPos = (cameraReverseRotation * vec4(f_eyePos,1.0)).xyz + cameraPos;

        vec3 f_boxPos = f_worldPos - world_cubeGenPos;

        vec3 invDir = 1.0 / reflectDir_world;

        int signX = int(1.0 - step(0.0, invDir.x));
        int signY = int(1.0 - step(0.0, invDir.y));
        int signZ = int(1.0 - step(0.0, invDir.z));

        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        vec3 bounds[2];

        bounds[0] = -boxSideHalf;
        bounds[1] = boxSideHalf;

        tmin = (bounds[signX].x - f_boxPos.x) * invDir.x;
        tmax = (bounds[1-signX].x - f_boxPos.x) * invDir.x;

        tymin = (bounds[signY].y - f_boxPos.y) * invDir.y; 
        tymax = (bounds[1-signY].y - f_boxPos.y) * invDir.y;

        if ((tmin > tymax) || (tymin > tmax))
            return vec4(0.0,0.0,0.0,0.0); 

        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;
     
        tzmin = (bounds[signZ].z - f_boxPos.z) * invDir.z;
        tzmax = (bounds[1-signZ].z - f_boxPos.z) * invDir.z;
     
        if ((tmin > tzmax) || (tzmin > tmax))
            return vec4(0.0,0.0,0.0,0.0);
     
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        // We sample the cubemap from the "inside", so need to skip the
        // tmin intersection, which corresponds to the ray hitting the box
        // from the outside

        float t = tmax;

        if (t < 0.0)
        { 
            return vec4(0.0,0.0,0.0,0.0);
        } 

        vec3 intersect = f_boxPos + t * reflectDir_world;

        return vec4(intersect,1.0);
    }

    // Calculate parallax correction by determining which inner face of the box the reflected ray hits
    // Because of this the face normals point inwards
    vec4 ParallaxCorrection(vec3 f_eyePos, vec3 reflectDir_world)
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
            return vec4(resultXP.yzw, 1.0);
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
            return vec4(resultXN.yzw, 1.0);
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
            return vec4(resultYP.yzw,1.0);
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
            return vec4(resultYN.yzw,1.0);
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
            return vec4(resultZP.yzw,1.0);
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
            return vec4(resultZN.yzw,1.0);
        }

        // Ray didn't hit any face, so no lighting from this environment map
        return vec4(0.0,0.0,0.0,0.0);

    }

#else

    vec4 ParallaxCorrection(vec3 f_worldPos, vec3 reflectDir_world)
    {
        return vec4(reflectDir_world,1.0);
    }

#endif