/*****************************************************************************
*
* env-prove-sahder.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ENV_PROBE_SHADER_H
#define CENGINE_ENV_PROBE_SHADER_H

#include <memory>
#include <ceng.h>

#include "shader-program.h"
#include "engine-result.h"

#include "camera.h"


namespace CEngine
{
	class EnvProbeShader
	{
	public:
		std::shared_ptr<CEngine::ShaderProgram> program;

		Ceng::ShaderConstant* fs_windowWidth;
		Ceng::ShaderConstant* fs_windowHeight;

		Ceng::ShaderConstant* fs_xDilationDiv;
		Ceng::ShaderConstant* fs_yDilationDiv;

		Ceng::ShaderConstant* fs_zTermA;
		Ceng::ShaderConstant* fs_zTermB;

		Ceng::ShaderConstant* fs_gbufferColor;
		Ceng::ShaderConstant* fs_gbufferNormal;

		Ceng::ShaderConstant* fs_depthBuffer;

		Ceng::ShaderConstant* fs_reflectionEnv;
		Ceng::ShaderConstant* fs_diffuseEnv;

		Ceng::ShaderConstant* fs_maxEnvLOD;

		Ceng::ShaderConstant* fs_cameraReverse;

	public:
		EnvProbeShader();
		virtual ~EnvProbeShader();

		virtual EngineResult::value Init(std::shared_ptr<CEngine::ShaderProgram>& in_program);

		virtual EngineResult::value PrepareRender(Camera* camera)=0;
	};

	class EnvProbeShaderParallax : public EnvProbeShader
	{
	public:
		Ceng::ShaderConstant* fs_boundaryCenterWorldPos;
		Ceng::ShaderConstant* fs_cameraPos;

		Ceng::FLOAT32 boundaryCenterWorldPos[3];
		Ceng::FLOAT32 cameraPos[3];

	public:
		EnvProbeShaderParallax();
		~EnvProbeShaderParallax() override;

		EngineResult::value Init(std::shared_ptr<CEngine::ShaderProgram>& in_program) override;

		EngineResult::value PrepareRender(Camera* camera) override;
	};

	class EnvProbeShaderParallaxAABB : public EnvProbeShaderParallax
	{
	public:
		Ceng::ShaderConstant* fs_boxSideHalf;
		
		Ceng::FLOAT32 boxSideHalf[3];

	public:
		EnvProbeShaderParallaxAABB();
		~EnvProbeShaderParallaxAABB() override;

		EngineResult::value Init(std::shared_ptr<CEngine::ShaderProgram>& in_program) override;

		EngineResult::value PrepareRender(Camera* camera) override;
	};
}


#endif