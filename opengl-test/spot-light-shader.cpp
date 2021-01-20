/*****************************************************************************
*
* spot-light-shader.cpp
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#include "spot-light-shader.h"
#include "entity.h"

#include "shader-manager.h"
#include "shader-program.h"

#include "point-light-component.h"
#include "rotation-component.h"
#include "position-component.h"
#include "spot-light-component.h"

using namespace CEngine;

SpotLightShader::SpotLightShader()
{
}

SpotLightShader::SpotLightShader(ShaderManager &shaderManager)
{
	Ceng::CRESULT cresult;

	EngineResult::value eresult;

	eresult = shaderManager.CreateProgramFromFile("quad.vs", "light-spot.fs", program);

	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	InitCommonUniforms();

	cresult = shaderProgram->GetConstant("lightDir", &u_lightDir);

	cresult = shaderProgram->GetConstant("coneCosOuter", &u_coneCosOuter);
	cresult = shaderProgram->GetConstant("coneCosInner", &u_coneCosInner);
}

SpotLightShader::~SpotLightShader()
{
	u_coneCosInner->Release();
	u_coneCosOuter->Release();
}

void SpotLightShader::Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraFullTransform,
	const Ceng::Matrix4 *cameraRotation,
	PointLightComponent *light, RotationComponent *rotation, SpotLightComponent *spot,
	PositionComponent *position)
{
	Ceng::VectorF4 posVec(position->x, position->y,position->z, 1.0f);

	Ceng::VectorF4 direction = rotation->rotationBasis[2];

	Ceng::VectorF4 finalDir = *cameraRotation * direction;

	Ceng::VectorF4 finalPos = *cameraFullTransform * posVec;

	u_lightPos->SetFloat3(&finalPos.x);

	u_lightDir->SetFloat3(&finalDir.x);

	u_coneCosOuter->SetFloat(spot->outerCos);
	u_coneCosInner->SetFloat(spot->innerCos);

	u_lightPower->SetFloat(light->power);

	u_lightCutoff->SetFloat(light->cutoffDist);

	context->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
}