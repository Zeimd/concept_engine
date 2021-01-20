#version 330 core

//#extension GL_NV_shadow_samplers_cube : enable

in vec2 f_uv;

uniform float windowWidth;
uniform float windowHeight;

uniform float zNear;
uniform float xDilationDiv; // 1.0 / dilation
uniform float yDilationDiv; // 1.0 / (dilation*aspectRatio)

uniform mat4 cameraReverseRotation;

uniform samplerCube envMap;

layout(location=0) out vec4 color;

vec3 ReconstructPos();

void main()
{
	vec3 fragPos = ReconstructPos();

	fragPos = normalize(fragPos);

	vec4 dir = cameraReverseRotation * vec4(fragPos,1.0);
	
	color = texture(envMap,dir.xyz);
	//color = textureLod(envMap,vec3(dir.x,dir.y,dir.z),0);
}

/**
 * Uses fragment screen position, depth buffer value and perspective
 * projection data to calculate fragment position in eye space.
 */
vec3 ReconstructPos()
{
	float depth = 0.0; // Depth buffer near value

	// Window space coordinates are in range [0,1]
	float zNDC = depth*2.0-1.0;

	//float zEye = 2*zNear*zFar / ( zNDC*(zFar-zNear)-zFar-zNear);
	// zTermA = 2*zNear*zFar / (zFar-zNear)
	// zTermB = (zFar+zNear)/(zFar-zNear)
	
	//float zEye = zTermA / (zNDC - zTermB);
	float zEye = -zNear;

	// Convert window space position to NDC coordinates
	float xWindow = gl_FragCoord.x/windowWidth;
	float yWindow = gl_FragCoord.y/windowHeight;

	float xClip = xWindow*2.0-1.0;
	float yClip = yWindow*2.0-1.0;

	// Reverse perspective projection
	float xEye = -zEye*xClip * xDilationDiv;
	float yEye = -zEye*yClip * yDilationDiv;

	return vec3(xEye,yEye,zEye);
}