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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: windowWidth\n");
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("windowHeight", &fs_windowHeight);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: windowHeight\n");
		fs_windowWidth->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("xDilationDiv", &fs_xDilationDiv);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: xDilationDiv\n");
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("yDilationDiv", &fs_yDilationDiv);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: yDilationDiv\n");
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("zTermA", &fs_zTermA);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: zTermA\n");
		fs_windowWidth->Release();
		fs_windowHeight->Release();
		fs_xDilationDiv->Release();
		fs_yDilationDiv->Release();
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("zTermB", &fs_zTermB);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: zTermB\n");
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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: gbufferColor\n");
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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: gbufferNormal\n");
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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: depthBuffer\n");
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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: reflectionEnv\n");
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
		Ceng::Log::Print(cresult);
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: diffuseEnv\n");
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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: maxEnvLOD\n");
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
		Ceng::Log::Print("EnvProbeShader::Init: Failed to get shader constant: cameraReverseRotation\n");
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
	fs_world_cubeGenPos->Release();
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

	cresult = shaderProgram->GetConstant("world_cubeGenPos", &fs_world_cubeGenPos);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShaderParallax::Init: Failed to get shader constant: boundaryCenterWorldPos\n");
		return EngineResult::fail;
	}

	cresult = shaderProgram->GetConstant("cameraPos", &fs_cameraPos);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("EnvProbeShaderParallax::Init: Failed to get shader constant: cameraPos\n");
		fs_world_cubeGenPos->Release();
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
		Ceng::Log::Print("EnvProbeShaderParallaxAABB::Init: Failed to get shader constant: boxSideHalf\n");
		return EngineResult::fail;
	}

	return EngineResult::ok;
}