#version 330 core

in vec3 position;

// projection * cameraRot * cameraPos * objPos * objRot
uniform mat4 fullTransform;

void main()
{
	gl_Position = fullTransform * vec4(position,1.0);
}