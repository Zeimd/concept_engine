#version 330 core

in vec3 f_eyePos;
in vec3 f_normal;
in vec3 f_tangent;
in vec2 f_uv;
in vec2 f_lmapUV;

uniform vec3 cameraWorldPos;

uniform sampler2D diffuseTex;
uniform sampler2D normalMap;
uniform sampler2D detailNormalMap;

uniform vec2 uvOffset;
uniform vec2 uvScale;

#ifdef UV_OFFSET

	vec2 GetOffsetUV()
	{
		return uvOffset;
	}

#else

	vec2 GetOffsetUV()
	{
		return vec2(0.0,0.0);
	}

#endif

#ifdef UV_SCALE

	vec2 GetScaleUV()
	{
		return uvScale;
	}

#else

	vec2 GetScaleUV()
	{
		return vec2(1.0,1.0);
	}

#endif

#if defined(UV_OFFSET) || defined(UV_SCALE)

	vec2 GetUV()
	{
		return (f_uv+GetOffsetUV())*GetScaleUV();
	}

#else

	vec2 GetUV()
	{
		return f_uv;
	}

#endif

uniform float detailScale;

layout(location=0) out vec4 out_color;
layout(location=1) out vec4 out_normal;
layout(location=2) out vec4 out_lightAccumulator;

#ifdef LIGHTMAP_SCALAR

	uniform sampler2D lightMap;

	// Need to write lightmap values to light accumulator
	// buffer here, so add it as output

	#include "lighting-math.fs"

#else

#endif

#ifdef UNIFORM_COLOR

	uniform vec3 diffuseColor;

	vec4 DiffuseColor(sampler2D texture,vec2 uv)
	{
		return vec4(diffuseColor,1.0);
	}

#else

	vec4 DiffuseColor(sampler2D texture,vec2 uv)
	{
		return texture2D(texture,uv);
	}

#endif

/*
#ifndef MATERIAL_UNIFORM
#define MATERIAL_TEXTURE
#endif
*/

#if defined(MATERIAL_TEXTURE)

	#define MATERIAL_TEX_GR

	uniform sampler2D materialTex;

	void GetMaterialData(out float metallic,out float roughness)
	{
		#if defined(MATERIAL_TEX_GR)
			vec2 temp = texture2D(materialTex,GetUV()).gr;
		#else
			vec2 temp = texture2D(materialTex,GetUV()).rg;
		#endif
		

		metallic = clamp(temp.r,0.02,0.99);
		roughness = clamp(temp.g,0.02,0.99);
	}

/*
	float Metallic()
	{
		//return 1.0;
		return clamp(texture2D(materialTex,GetUV()).r,0.02,0.99);
	}

	float Roughness()
	{
		//return 0.2;
		return clamp(texture2D(materialTex,GetUV()).g,0.02,0.99);
	}
	*/

#elif defined(MATERIAL_UNIFORM) 

	uniform float materialMetallic;
	uniform float materialRoughness;

	void GetMaterialData(out float metallic,out float roughness)
	{
		metallic = clamp(materialMetallic,0.02,0.99);
		roughness = clamp(materialRoughness,0.02,0.99);	
	}

/*
	float Metallic()
	{
		return clamp(materialMetallic,0.02,0.99);
	}

	float Roughness()
	{
		return clamp(materialRoughness,0.02,0.99);
	}
	*/

#else

	float materialMetallic = 0.9;
	float materialRoughness = 0.2;

	void GetMaterialData(out float metallic,out float roughness)
	{
		metallic = clamp(materialMetallic,0.02,0.99);
		roughness = clamp(materialRoughness,0.02,0.99);	
	}

/*
	float Metallic()
	{
		return clamp(materialMetallic,0.02,0.99);
	}

	float Roughness()
	{
		return clamp(materialRoughness,0.02,0.99);
	}
	*/

#endif

float windowWidth = 640.0;
float windowHeight = 480.0;

vec3 GetNormal(mat3 tbn,sampler2D texture,vec2 uv);

vec3 EncodeColor3(vec3 color);

vec2 EncodeColor2(vec3 color);

void main()
{
	// Tangent and normal in eye space
	
	vec3 normal = normalize(f_normal);
	vec3 tangent = normalize(f_tangent);

	// Orthogonalize normal and tangent

	tangent = normalize(tangent - dot(tangent,normal)*normal);

	vec3 bitangent = cross(tangent,normal);

	mat3 tbn = mat3(tangent,bitangent,normal);

	vec2 uv = GetUV();

	normal = GetNormal(tbn,normalMap,f_uv);

	vec3 detailNormal = GetNormal(tbn,detailNormalMap,f_uv*detailScale);

	// Only blend x and y components of the normal
	//normal.xy += detailNormal.xy;
	//normal.z *= detailNormal.z;

	normal = normalize(normal);

	float zSign = sign(normal.z);

	// Convert normal to [0,1] range for texture storage
	normal = 0.5*(normal+vec3(1.0,1.0,1.0));

	// Encode sign of surface normal's z-component with material's 
	// metallic value, which is known to be >= 0. This means that the only
	// way the resulting value can be negative is if zSign < 0.

	float metallic,roughness;

	GetMaterialData(metallic,roughness);

	//float storeMetallic = zSign + Metallic();
	float storeMetallic = zSign*metallic;
	storeMetallic = 0.5*storeMetallic+0.5;

	vec3 color = DiffuseColor(diffuseTex,f_uv).rgb;

	out_color = vec4(color,storeMetallic);

	// Only store X and Y component of surface normal explicitly.
	//out_normal = vec4(normal.xy,0.0,Roughness());

	out_normal = vec4(normal.xyz,roughness);

#ifdef LIGHTMAP_SCALAR
	LightmapContribution(color,normal,f_lmapUV,metallic,roughness);
	//out_lightAccumulator = vec4(0,0,0,1);
#else
	out_lightAccumulator = vec4(0,0,0,1);
#endif
}



vec3 EncodeColor_YCbCr(vec3 color)
{
	float y = 0.299*color.r + 0.587*color.g + 0.114*color.b;

	float cb = 0.5 - 0.168*color.r - 0.331*color.g + 0.5*color.b;
	float cr = 0.5 + 0.5*color.r - 0.418*color.g - 0.081*color.b;

	return vec3(y,cb,cr);
}

#if defined(NO_NORMAL_MAP)

	vec3 GetNormal(mat3 tbn,sampler2D texture,vec2 uv)
	{
		return normalize(tbn * vec3(0.0,0.0,1.0));
	}

#elif defined(NORMAL_MAP_XYZ)

	// Normal map has all 3 components
	// Stored vector is not required to be unit length.

	vec3 GetNormal(mat3 tbn,sampler2D texture,vec2 uv)
	{
		vec3 read = texture2D(texture,uv).rgb;

		// X and y components are in range [0,1], so
		// convert them to the correct [-1,1] range.
		read.xy = 2.0*read.xy - vec2(1.0,1.0);

		// Assume: z component in [0,1] range

		return normalize(tbn * read);
	}

#else

	// Normal map only provides x- and y-components.
	// Z-component is reconstructed with the assumption
	// that the stored vector is unit length.

	//#define NORMAL_MAP_AG
	#define NORMAL_MAP_GR

	vec3 GetNormal(mat3 tbn,sampler2D texture,vec2 uv)
	{

		#if defined(NORMAL_MAP_AG)
			// x component is in texture's alpha channel
			// Used with dxt5nm
			vec2 read = texture2D(texture,uv).ag;
		#elif defined(NORMAL_MAP_GR)
			vec2 read = texture2D(texture,uv).gr;
		#else
			// x component is in texture's red channel
			vec2 read = texture2D(texture,uv).rg;
		#endif

		#ifndef NORMAL_MAP_SIGNED
			// x and y components are in [0,1] range,
			// so convert them to [-1,1]
			read = 2.0*read - vec2(1.0,1.0);
		#endif

		// Assume: z-component in [0,1] range

		float z = sqrt(1.0 - dot(read,read));

		vec3 bumpNormal = vec3(read,z);

		// Transform normal read from normal map to eye space

		return normalize(tbn * bumpNormal);
	}

#endif