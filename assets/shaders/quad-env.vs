#version 330 core

in vec2 position;
in vec2 textureUV;

out vec2 f_uv;

void main()
{
	gl_Position = vec4(position,1.0,1.0);

	f_uv = textureUV;
}