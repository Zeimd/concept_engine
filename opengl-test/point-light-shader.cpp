/*****************************************************************************
*
* point-light-shader.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include <ceng.h>

#include "position-component.h"
#include "point-light-component.h"
#include "shader-manager.h"
#include "shader-program.h"
#include "point-light-shader.h"

using namespace CEngine;

PointLightShader::PointLightShader()
{
}

PointLightShader::PointLightShader(ShaderManager &shaderManager)
{
	EngineResult::value eresult;

	eresult = shaderManager.CreateProgramFromFile("quad.vs", "light-point.fs", program);

	InitCommonUniforms();
}

void PointLightShader::InitCommonUniforms()
{
	Ceng::CRESULT cresult;

	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	cresult = shaderProgram->GetConstant("windowWidth", &u_windowWidth);
	cresult = shaderProgram->GetConstant("windowHeight", &u_windowHeight);
	cresult = shaderProgram->GetConstant("xDilationDiv", &u_xDilationDiv);
	cresult = shaderProgram->GetConstant("yDilationDiv", &u_yDilationDiv);
	cresult = shaderProgram->GetConstant("zTermA", &u_zTermA);
	cresult = shaderProgram->GetConstant("zTermB", &u_zTermB);
	cresult = shaderProgram->GetConstant("gbufferColor", &u_gbufferColor);
	cresult = shaderProgram->GetConstant("gbufferNormal", &u_gbufferNormal);
	cresult = shaderProgram->GetConstant("depthBuffer", &u_depthBuffer);
	cresult = shaderProgram->GetConstant("lightPos", &u_lightPos);
	cresult = shaderProgram->GetConstant("lightPower", &u_lightPower);
	cresult = shaderProgram->GetConstant("cutoffDistance", &u_lightCutoff);
}

PointLightShader::~PointLightShader()
{
	u_windowWidth->Release();
	u_windowHeight->Release();

	u_xDilationDiv->Release();
	u_yDilationDiv->Release();

	u_zTermA->Release();
	u_zTermB->Release();

	u_gbufferColor->Release();
	u_gbufferNormal->Release();

	u_depthBuffer->Release();
	u_lightPos->Release();

	u_lightPower->Release();
	u_lightCutoff->Release();
}

void PointLightShader::Configure(Ceng::RenderContext *context,const Ceng::UINT32 displayWidth, const Ceng::UINT32 displayHeight,
	const Ceng::FLOAT32 xDilationDiv, const Ceng::FLOAT32 yDilationDiv,
	const Ceng::FLOAT32 zTermA, const Ceng::FLOAT32 zTermB, const Ceng::INT32 colorBufferTex,
	const Ceng::INT32 normalBufferTex, const Ceng::INT32 depthBufferTex)
{
	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	context->SetShaderProgram(shaderProgram);

	u_windowWidth->SetFloat((Ceng::FLOAT32)displayWidth);
	u_windowHeight->SetFloat((Ceng::FLOAT32)displayHeight);

	u_xDilationDiv->SetFloat(xDilationDiv);
	u_yDilationDiv->SetFloat(yDilationDiv);
	u_zTermA->SetFloat(zTermA);
	u_zTermB->SetFloat(zTermB);

	u_gbufferColor->SetInt(colorBufferTex);
	u_gbufferNormal->SetInt(normalBufferTex);
	u_depthBuffer->SetInt(depthBufferTex);
}

void PointLightShader::Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraFullTransform,
	PointLightComponent *light)
{
	Ceng::VectorF4 position(light->position->x, light->position->y,
		light->position->z,1.0f);

	Ceng::VectorF4 finalPos = *cameraFullTransform * position;

	u_lightPos->SetFloat3(&finalPos.x);

	u_lightPower->SetFloat(light->power);

	u_lightCutoff->SetFloat(light->cutoffDist);

	context->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
}


