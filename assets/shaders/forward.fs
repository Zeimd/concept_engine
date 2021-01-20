#version 330 core

in vec3 f_normal;
in vec3 f_tangent;
in vec2 f_uv;

uniform vec3 cameraWorldPos;

uniform sampler2D diffuseTex;
uniform sampler2D normalMap;

uniform float zTermA;
uniform float zTermB;
uniform float xDilationDiv; // 1.0 / dilation
uniform float yDilationDiv; // 1.0 / (dilation*aspectRatio)

out vec4 color;

float twopiDiv = 0.15915;
float piDiv = 0.31831;

float windowWidth = 640.0;
float windowHeight = 480.0;

uniform vec3 lightPos;

vec3 lightColor = vec3(1.0,1.0,1.0);

float lightPower = 10.0;

float materialMetallic = 0.01;
float materialRoughness = 0.75;

float Metallic();

float Roughness();

vec3 ReconstructPos();

vec3 BlendColor(vec3 surfaceColor,vec3 diffuseColor,vec3 specularColor);

vec3 GetNormal();

float SpecularPhong(vec3 toLight,vec3 normal,vec3 toEye,float roughness);

float FresnelTerm(vec3 halfway,vec3 toLight);

float BaseReflectivity(float metallic);

void main()
{
	vec3 f_eyePos = ReconstructPos();

	vec3 toLight = lightPos - f_eyePos;

	// Light attentuation 1/r^2
	float distSquared = dot(toLight,toLight);
	float inverseDist = inversesqrt(distSquared);

	float powerTerm = inverseDist*lightPower;

	toLight = normalize(toLight);

	vec3 normal = GetNormal();

	// Specular term

	vec3 toEye = normalize(-f_eyePos);

	vec3 halfway = normalize(toLight+toEye);

	float specularPower = SpecularPhong(toLight,normal,toEye,Roughness());

	specularPower *= powerTerm;

	// Fresnel term

	float fresnel = FresnelTerm(halfway,toLight);

	specularPower *= fresnel;

	vec3 specularColor = lightColor * specularPower;

	// Diffuse term

	float diffusePower = max(0,powerTerm - specularPower); // energy conservation

	float lambert = dot(toLight,normal);
	lambert = clamp(lambert,0,1);

	vec3 baseColor = texture2D(diffuseTex,f_uv).rgb;
	
	vec3 diffuseColor = lambert*baseColor*diffusePower;

	vec3 finalColor = BlendColor(baseColor,diffuseColor,specularColor);

	color = vec4(finalColor,1.0);
}

float SpecularPhong(vec3 toLight,vec3 normal,vec3 toEye,float roughness)
{	
	float alpha = roughness*roughness;

	float scaling = 2*(1.0/(alpha*alpha)-1.0);

	vec3 halfway = normalize(toLight+toEye);

	float halfCos = dot(halfway,normal);

	float clip = step(0.0,halfCos);

	halfCos = max(0,halfCos);

	float specular = clip*(scaling+2.0)*twopiDiv*pow(halfCos,scaling);

	return specular;
}

float BaseReflectivity(float metallic)
{
	return clamp(metallic,0.01,0.95);

	//return materialFresnel;
}

float FresnelTerm(vec3 halfway,vec3 toLight)
{
	float factor = BaseReflectivity(Metallic());

	// Schlick's approximation

	float fresnel = factor+(1.0-factor)*pow(1-dot(halfway,toLight),5.0);

	//fresnel = clamp(fresnel,0.01,0.99);
	return fresnel;
}

vec3 BlendColor(vec3 surfaceColor,vec3 diffuseColor,vec3 specularColor)
{
	// Semiconductors have metallic value in the range [0.2,0.45]
	// If MetallicTerm() < 0.2, assume dielectric
	float semiconductorMix = smoothstep(0.2,0.45,Metallic());

	vec3 dielectricColor = diffuseColor + specularColor;
	vec3 metalColor = surfaceColor * specularColor;

	return mix(dielectricColor,metalColor,semiconductorMix);
}

float Roughness()
{
	return clamp(materialRoughness,0.1,1);
}

float Metallic()
{
	return materialMetallic;
}

vec3 GetNormal()
{
	// Tangent and normal in world space
	
	vec3 normal = normalize(f_normal);
	vec3 tangent = normalize(f_tangent);

	// Orthogonalize normal and tangent

	tangent = normalize(tangent - dot(tangent,normal)*normal);

	vec3 bitangent = cross(tangent,normal);

	vec3 bumpNormal = texture2D(normalMap,f_uv).rgb*2.0-vec3(1.0,1.0,1.0);

	// Transform normal read from normal map to world space

	mat3 tbn = mat3(tangent,bitangent,normal);

	return normalize(tbn * bumpNormal);
}

/**
 * Uses fragment screen position, depth buffer value and perspective
 * projection data to calculate fragment position in eye space.
 */
vec3 ReconstructPos()
{
	// Window space coordinates are in range [0,1]
	float zNDC = gl_FragCoord.z*2.0-1.0;

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