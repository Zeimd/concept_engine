#version 330 core

in vec2 f_uv;

uniform sampler2D texture;

layout(location=0) out vec4 color;

void main()
{
	vec3 temp = texture2D(texture,f_uv).rgb;

	/*
	float depth = temp.r;

	// Window space coordinates are in range [0,1]
	float zNDC = depth*2.0-1.0;

	float zNear = 0.01;
	float zFar = 1000;

	float zEye = 2*zNear*zFar / ( zNDC*(zFar-zNear)-zFar-zNear);
	//float zTermA = 2*zNear*zFar / (zFar-zNear)
	//float zTermB = (zFar+zNear)/(zFar-zNear)
	//float zEye = zTermA / (zNDC - zTermB);

	zEye = abs(zEye)/10;

	color = vec4(zEye,zEye,zEye,1.0);
	*/

	temp = temp / (temp+vec3(1.0,1.0,1.0));
	color = vec4(temp,1.0);
}