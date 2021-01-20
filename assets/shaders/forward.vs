#version 330 core

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec2 textureUV;

// projection * cameraRot * cameraPos * objPos * objRot
uniform mat4 fullTransform;

uniform mat4 toEyeSpace; // cameraRot * cameraPos * objPos * objRot

uniform mat4 eyeObjectRotation; // cameraRot * objRot

out vec3 f_eyePos; // fragment position in eye space

out vec3 f_normal; // normal in eye space
out vec3 f_tangent; // tangent in eye space

out vec2 f_uv;

void main()
{
	gl_Position = fullTransform * vec4(position,1.0);

	f_normal = (eyeObjectRotation*vec4(normal,1.0)).xyz;
	f_tangent = (eyeObjectRotation*vec4(tangent,1.0)).xyz;

	f_uv = textureUV;
}