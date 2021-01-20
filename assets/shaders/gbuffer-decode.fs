/*****************************************************************************
*
* gbuffer-decode.fs
*
* Created By Jari Korkala 13/12/2015
*
* This file is not a valid fragment shader, but contains common
* declarations and functions.
*
*****************************************************************************/

in vec2 f_uv;

uniform sampler2D gbufferColor;
uniform sampler2D gbufferNormal;
uniform sampler2D depthBuffer;

uniform float zTermA;
uniform float zTermB;
uniform float xDilationDiv; // 1.0 / dilation
uniform float yDilationDiv; // 1.0 / (dilation*aspectRatio)

uniform float windowWidth;
uniform float windowHeight;

/**
 * Uses fragment screen position, depth buffer value and perspective
 * projection data to calculate fragment position in eye space.
 */
vec3 ReconstructPos()
{
	float depth = texture2D(depthBuffer,f_uv).r;

	if (depth == 1.0) discard;

	// Window space coordinates are in range [0,1]
	float zNDC = depth*2.0-1.0;

	//float zEye = 2*zNear*zFar / ( zNDC*(zFar-zNear)-zFar-zNear);
	// zTermA = 2*zNear*zFar / (zFar-zNear)
	// zTermB = (zFar+zNear)/(zFar-zNear)
	float zEye = zTermA / (zNDC - zTermB);

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

/**
 * Read diffuse color (or specular color for metals), surface normal and material
 * parameters from g-buffer.
 */
void ReadGBuffer(out vec3 baseColor,out vec3 normal,out float metallic,out float roughness)
{
	// Read data from color buffer
	vec4 colorRead = texture2D(gbufferColor,f_uv);

	baseColor = colorRead.rgb;

	// Decode normal vector z-sign and material's metallic value

	float temp = colorRead.a;

	temp = 2.0*temp-1.0;

	float zSign = sign(temp);

	metallic = abs(temp);

	// Read data from normal buffer

	vec4 normalRead = texture2D(gbufferNormal,f_uv);

/*
	vec2 normalBase = normalRead.xy;

	normalBase = 2.0*normalBase - vec2(1.0,1.0);

	// Surface normal vector is unit length, so z-component can be
	// recovered
	normal = vec3(normalBase,zSign*sqrt(1.0-dot(normalBase,normalBase)));
*/

	normal = 2.0*normalRead.xyz - vec3(1.0,1.0,1.0);

	roughness = normalRead.w;
}

vec3 DecodeColor_YCbCr(vec3 color)
{
	float y = color.r;

	float cb = color.g;
	float cr = color.b;

	float r = y + 1.402*(cr-0.5);
	float g = y - 0.344*(cb-0.5)-0.714*(cr-0.5);
	float b = y + 1.772*(cb-0.5);

	return vec3(r,g,b);
}
