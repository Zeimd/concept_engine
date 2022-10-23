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

EngineResult::value EnvProbeShader::GetInstance(std::shared_ptr<CEngine::ShaderProgram>& in_program, 
	EnvProbeShader** output)
{
	EnvProbeShader* shader = new EnvProbeShader();

	CEngine::EngineResult::value eresult;

	shader->program = in_program;

	eresult = shader->Init();

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	*output = shader;

	return EngineResult::ok;
}

EngineResult::value EnvProbeShader::Init()
{
	Ceng::ShaderProgram* shaderProgram = program->GetProgram();

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("windowWidth", &fs_windowWidth);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("windowHeight", &fs_windowHeight);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("xDilationDiv", &fs_xDilationDiv);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("yDilationDiv", &fs_yDilationDiv);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("zTermA", &fs_zTermA);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		fs_yDilationDiv->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("zTermB", &fs_zTermB);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		fs_yDilationDiv->Release();
		fs_zTermA->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("gbufferColor", &fs_gbufferColor);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		fs_yDilationDiv->Release();
		fs_zTermA->Release();	
		fs_zTermB->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("gbufferNormal", &fs_gbufferNormal);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		fs_yDilationDiv->Release();
		fs_zTermA->Release();
		fs_zTermB->Release();
		fs_gbufferColor->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("depthBuffer", &fs_depthBuffer);
	if (cresult != Ceng::CE_OK)
	{
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		fs_yDilationDiv->Release();
		fs_zTermA->Release();
		fs_zTermB->Release();
		fs_gbufferColor->Release();
		fs_gbufferNormal->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("reflectionEnv", &fs_reflectionEnv);
	if (cresult != Ceng::CE_OK)
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
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("diffuseEnv", &fs_diffuseEnv);
	if (cresult != Ceng::CE_OK)
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
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("maxEnvLOD", &fs_maxEnvLOD);
	if (cresult != Ceng::CE_OK)
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
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("cameraReverseRotation", &fs_cameraReverse);
	if (cresult != Ceng::CE_OK)
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

		return EngineResult::fail;
	}

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

EngineResult::value EnvProbeShaderParallax::Init()
{
	EngineResult::value eresult;

	eresult = EnvProbeShader::Init();

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	Ceng::ShaderProgram* shaderProgram = program->GetProgram();

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("boundaryCenterWorldPos", &fs_boundaryCenterWorldPos);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("cameraPos", &fs_cameraPos);
	if (cresult != Ceng::CE_OK)
	{
		fs_boundaryCenterWorldPos->Release();
		return EngineResult::fail;
	}

	return EngineResult::ok;
}

EnvProbeShaderParallaxAABB::EnvProbeShaderParallaxAABB()
{

}

EnvProbeShaderParallaxAABB::~EnvProbeShaderParallaxAABB()
{
	fs_boxSideHalf->Release();
}

EngineResult::value EnvProbeShaderParallaxAABB::GetInstance(std::shared_ptr<CEngine::ShaderProgram>& in_program, 
	EnvProbeShaderParallaxAABB** output)
{
	EnvProbeShaderParallaxAABB* shader = new EnvProbeShaderParallaxAABB();

	CEngine::EngineResult::value eresult;

	shader->program = in_program;

	eresult = shader->Init();

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	*output = shader;

	return EngineResult::ok;
}

EngineResult::value EnvProbeShaderParallaxAABB::Init()
{
	EngineResult::value eresult;

	eresult = EnvProbeShaderParallax::Init();

	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	Ceng::ShaderProgram* shaderProgram = program->GetProgram();

	Ceng::CRESULT cresult;

	cresult = shaderProgram->GetConstant("boxSideHalf", &fs_boxSideHalf);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	return EngineResult::ok;
}