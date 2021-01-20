/*****************************************************************************
*
* spot-shadow-shader.cpp
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#include "spot-shadow-shader.h"
#include "shader-manager.h"
#include "shader-program.h"
#include "shadowmap-component.h"

#include "position-component.h"
#include "rotation-component.h"
#include "point-light-component.h"
#include "spot-light-component.h"

using namespace CEngine;

SpotShadowShader::SpotShadowShader()
{
}


SpotShadowShader::SpotShadowShader(ShaderManager &shaderManager)
{
	Ceng::CRESULT cresult;

	EngineResult::value eresult;

	eresult = shaderManager.CreateProgramFromFile("quad.vs", "light-spot-shadow.fs", program);

	//if (eresult != EngineResult::ok)
	//{
	Ceng::Log::Print(program->managerLog);
	//}

	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	InitCommonUniforms();

	cresult = shaderProgram->GetConstant("lightDir", &u_lightDir);

	cresult = shaderProgram->GetConstant("coneCosOuter", &u_coneCosOuter);
	cresult = shaderProgram->GetConstant("coneCosInner", &u_coneCosInner);

	cresult = shaderProgram->GetConstant("shadowMap", &u_shadowMap);
	cresult = shaderProgram->GetConstant("lightProjection", &u_lightProjection);
	cresult = shaderProgram->GetConstant("cameraTransform", &u_cameraTransform);

	cresult = shaderProgram->GetConstant("depthBias", &u_depthBias);
}

SpotShadowShader::~SpotShadowShader()
{
	u_shadowMap->Release();
	u_lightProjection->Release();
	u_cameraTransform->Release();
	u_depthBias->Release();
}

void SpotShadowShader::Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraFullTransform,
	const Ceng::Matrix4 *cameraRotation,
	PointLightComponent *light, RotationComponent *rotation, SpotLightComponent *spot,
	PositionComponent *position, ShadowmapComponent *shadow, Ceng::SamplerState *shadowSampler)
{
	Ceng::Matrix4 lightProjection;

	lightProjection = shadow->projection * shadow->lightTransform * cameraFullTransform->Inverse();

	u_lightProjection->SetMatrix_4x4(&lightProjection.data[0][0], true);

	// TODO: invert before shading
	u_cameraTransform->SetMatrix_4x4(&cameraFullTransform->data[0][0], true);

	Ceng::VectorF4 posVec(position->x, position->y, position->z, 1.0f);

	Ceng::VectorF4 finalPos = *cameraFullTransform * posVec;

	u_lightPos->SetFloat3(&finalPos.x);

	Ceng::VectorF4 finalDir = -rotation->rotationBasis[2];

	finalDir = *cameraRotation * finalDir;

	u_lightDir->SetFloat3(&finalDir.x);

	u_coneCosOuter->SetFloat(spot->outerCos);
	u_coneCosInner->SetFloat(spot->innerCos);

	u_lightPower->SetFloat(light->power);

	u_lightCutoff->SetFloat(light->cutoffDist);

	u_shadowMap->SetInt(3);

	u_depthBias->SetFloat(shadow->depthBias);

	context->SetPixelShaderResource(3, shadow->depthView);
	context->SetPixelShaderSamplerState(3, shadowSampler);

	context->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
}


