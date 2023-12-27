/*****************************************************************************
*
* env-probe.cpp
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#include "env-probe.h"

using namespace CEngine;

EnvProbe::EnvProbe()
{

}

EnvProbe::~EnvProbe()
{
	if (envMapView != nullptr)
	{
		envMapView->Release();
	}

	if (irradianceMapView != nullptr)
	{
		irradianceMapView->Release();
	}
}

CEngine::EnvProbeShader* EnvProbe::GetProgram()
{
	return nullptr;
}

void EnvProbe::PrepareRender(const Vec3& cameraPos)
{

}

//////////////////////////////////////////////////////////

EnvProbeAABOX::EnvProbeAABOX()
{

}

EnvProbeAABOX::~EnvProbeAABOX()
{

}

CEngine::EnvProbeShader* EnvProbeAABOX::GetProgram()
{
	return program.get();
}

void EnvProbeAABOX::PrepareRender(const Vec3& cameraPos)
{
	program->fs_boxSideHalf->SetFloat3((Ceng::FLOAT32*)&boxSideHalf);

	program->fs_world_cubeGenPos->SetFloat3((Ceng::FLOAT32*)&world_cubeGenPos);

	program->fs_cameraPos->SetFloat3((Ceng::FLOAT32*)&cameraPos);
}

