/*****************************************************************************
*
* env-map.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include "env-probe-shader.h"
#include "math-vector.h"

using namespace CEngine;

EnvProbeShader::EnvProbeShader()
{

}

EnvProbeShader::~EnvProbeShader()
{
	fs_windowWidth->Release();
	fs_windowHeight->Release();

	fs_xDilationDiv->Release();
	fs_yDilationDiv->Release();

	fs_zTermA->Release();
	fs_zTermB->Release();

	fs_gbufferColor->Release();
	fs_gbufferNormal->Release();

	fs_depthBuffer->Release();

	fs_reflectionEnv->Release();
	fs_diffuseEnv->Release();

	fs_maxEnvLOD->Release();
	fs_cameraReverse->Release();
}

EngineResult::value EnvProbeShader::Init(std::shared_ptr<CEngine::ShaderProgram>& in_program)
{
	program = in_program;

	Ceng::ShaderProgram* shaderProgram = program->GetProgram();

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("windowWidth", &fs_windowWidth);

	cresult = shaderProgram->GetConstant("windowHeight", &fs_windowHeight);

	cresult = shaderProgram->GetConstant("xDilationDiv", &fs_xDilationDiv);

	cresult = shaderProgram->GetConstant("yDilationDiv", &fs_yDilationDiv);

	cresult = shaderProgram->GetConstant("zTermA", &fs_zTermA);

	cresult = shaderProgram->GetConstant("zTermB", &fs_zTermB);

	cresult = shaderProgram->GetConstant("gbufferColor", &fs_gbufferColor);

	cresult = shaderProgram->GetConstant("gbufferNormal", &fs_gbufferNormal);

	cresult = shaderProgram->GetConstant("depthBuffer", &fs_depthBuffer);

	cresult = shaderProgram->GetConstant("reflectionEnv", &fs_reflectionEnv);

	cresult = shaderProgram->GetConstant("diffuseEnv", &fs_diffuseEnv);

	cresult = shaderProgram->GetConstant("maxEnvLOD", &fs_maxEnvLOD);

	cresult = shaderProgram->GetConstant("cameraReverseRotation", &fs_cameraReverse);

	return EngineResult::ok;
}

EnvProbeShaderParallax::EnvProbeShaderParallax()
{

}
EnvProbeShaderParallax::~EnvProbeShaderParallax()
{
	fs_boundaryCenterWorldPos->Release();
	fs_cameraPos->Release();
}

EngineResult::value EnvProbeShaderParallax::Init(std::shared_ptr<CEngine::ShaderProgram>& in_program)
{
	EngineResult::value eresult;

	eresult = EnvProbeShader::Init(in_program);

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	Ceng::ShaderProgram* shaderProgram = program->GetProgram();

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("boundaryCenterWorldPos", &fs_boundaryCenterWorldPos);

	cresult = shaderProgram->GetConstant("cameraPos", &fs_cameraPos);

	return EngineResult::ok;
}

EngineResult::value EnvProbeShaderParallax::PrepareRender(Camera* camera)
{
	fs_boundaryCenterWorldPos->SetFloat3(boundaryCenterWorldPos);

	Vec3 cameraPos;

	camera->GetPosition(&cameraPos);

	fs_cameraPos->SetFloat3((Ceng::FLOAT32*)&cameraPos);

	return EngineResult::ok;
}

EnvProbeShaderParallaxAABB::EnvProbeShaderParallaxAABB()
{

}

EnvProbeShaderParallaxAABB::~EnvProbeShaderParallaxAABB()
{
	fs_boxSideHalf->Release();
}

EngineResult::value EnvProbeShaderParallaxAABB::Init(std::shared_ptr<CEngine::ShaderProgram>& in_program)
{
	EngineResult::value eresult;

	eresult = EnvProbeShaderParallax::Init(in_program);

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	Ceng::ShaderProgram* shaderProgram = program->GetProgram();

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("boxSideHalf", &fs_boxSideHalf);

	return EngineResult::ok;
}

EngineResult::value EnvProbeShaderParallaxAABB::PrepareRender(Camera* camera)
{
	EngineResult::value eresult;

	eresult = EnvProbeShaderParallax::PrepareRender(camera);

	fs_boxSideHalf->SetFloat3(boxSideHalf);

	return EngineResult::ok;
}



