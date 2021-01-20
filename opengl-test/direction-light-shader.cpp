/*****************************************************************************
*
* direction-light-shader.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "direction-light-shader.h"

#include "point-light-component.h"

#include "shader-manager.h"
#include "shader-program.h"

#include "rotation-component.h"

using namespace CEngine;

DirectionLightShader::DirectionLightShader()
{
}

DirectionLightShader::DirectionLightShader(ShaderManager &shaderManager)
{
	Ceng::CRESULT cresult;

	EngineResult::value eresult;

	eresult = shaderManager.CreateProgramFromFile("quad.vs", "light-dir.fs", program);

	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	InitCommonUniforms();

	cresult = shaderProgram->GetConstant("lightDir", &u_lightDir);
}

DirectionLightShader::~DirectionLightShader()
{
	u_lightDir->Release();
}

void DirectionLightShader::Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraRotation,
	PointLightComponent *light,RotationComponent *rotation)
{
	Ceng::VectorF4 direction = rotation->rotationBasis[2];

	Ceng::VectorF4 finalDir = *cameraRotation * direction;

	u_lightDir->SetFloat3(&finalDir.x);

	u_lightPower->SetFloat(light->power);

	context->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
}