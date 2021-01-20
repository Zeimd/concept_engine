#version 330 core

in vec3 f_normal;
in vec3 f_tangent;
in vec2 f_uv;

uniform vec3 cameraWorldPos;

uniform sampler2D diffuseTex;
uniform sampler2D normalMap;

layout(location=0) out vec4 out_color;
layout(location=1) out vec4 out_normal;

float materialMetallic = 0.1;
float materialRoughness = 0.75;

float Metallic();

float Roughness();

vec3 GetNormal();

void main()
{
	vec3 normal = GetNormal();

	// Convert normal to [0,1] range for texture storage
	normal = 0.5*(normal+vec3(1.0,1.0,1.0));

	vec3 color = texture2D(diffuseTex,f_uv).rgb;

	out_color = vec4(color,Metallic());

	out_normal = vec4(normal,Roughness());
}

float Roughness()
{
	//return clamp(materialRoughness,0.1,1);
	return clamp(materialRoughness,0,1);
}

float Metallic()
{
	return clamp(materialMetallic,0,1);
}

vec3 GetNormal()
{
	// Tangent and normal in eye space
	
	vec3 normal = normalize(f_normal);
	vec3 tangent = normalize(f_tangent);

	// Orthogonalize normal and tangent

	tangent = normalize(tangent - dot(tangent,normal)*normal);

	vec3 bitangent = cross(tangent,normal);

	vec3 bumpNormal = texture2D(normalMap,f_uv).rgb*2.0-vec3(1.0,1.0,1.0);

	// Transform normal read from normal map to eye space

	mat3 tbn = mat3(tangent,bitangent,normal);

	return normalize(tbn * bumpNormal);
}
