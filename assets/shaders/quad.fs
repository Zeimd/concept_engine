#version 330 core

in vec2 f_uv;

uniform sampler2D texture;

out vec4 color;

void main()
{
	color = texture2D(texture,f_uv);
	//color = vec4(1.0,0,0,1.0);
}