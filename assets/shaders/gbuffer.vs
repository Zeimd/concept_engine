#version 330 core

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec2 textureUV;

in vec2 lightmapUV; // Used when lightmaps are enabled

// projection * cameraRot * cameraPos * objPos * objRot
uniform mat4 fullTransform;

// cameraRot * cameraPos * objPos * objRot
uniform mat4 eyeTransform;

uniform mat4 eyeObjectRotation; // cameraRot * objRot

out vec3 f_eyePos; // fragment position in eye space
out vec3 f_normal; // normal in eye space
out vec3 f_tangent; // tangent in eye space

out vec2 f_uv;
out vec2 f_lmapUV;

void main()
{
	gl_Position = fullTransform * vec4(position,1.0);

	f_eyePos = (eyeTransform * vec4(position,1.0)).xyz;

	f_normal = (eyeObjectRotation*vec4(normal,1.0)).xyz;
	f_tangent = (eyeObjectRotation*vec4(tangent,1.0)).xyz;

	f_uv = textureUV;
	f_lmapUV = lightmapUV;
}