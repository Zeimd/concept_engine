/*****************************************************************************
*
* env-map-manager.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ENV_MAP_MANAGER_H
#define CENGINE_ENV_MAP_MANAGER_H

#include "manager.h"

#include "env-probe-shader.h"

#include "engine-result.h"

#include "render-params.h"

#include "texture.h"

namespace CEngine
{
	class TextureManager;
	class ShaderManager;

	class EnvProbe
	{
	public:

		Ceng::StringUtf8 name;

		std::shared_ptr<CEngine::Texture> envMap;
		std::shared_ptr<CEngine::Texture> irradianceMap;

		Ceng::ShaderResourceView* envMapView;
		Ceng::ShaderResourceView* irradianceMapView;

	public:
		EnvProbe()
		{

		}

		virtual ~EnvProbe()
		{

		}

		virtual CEngine::EnvProbeShader* GetProgram() = 0;

		virtual void PrepareRender(const Vec3& cameraPos) = 0;
	};

	class EnvProbeAABOX : public EnvProbe
	{
	public:

		std::shared_ptr<CEngine::EnvProbeShaderParallaxAABB> program;

		Vec3 boxSideHalf;
		Vec3 boundaryCenterWorldPos;

	public:
		EnvProbeAABOX()
		{

		}

		~EnvProbeAABOX() override
		{

		}

		CEngine::EnvProbeShader* GetProgram() override
		{
			return program.get();
		}

		void PrepareRender(const Vec3& cameraPos) override
		{
			program->fs_boxSideHalf->SetFloat3((Ceng::FLOAT32*)&boxSideHalf);

			program->fs_boundaryCenterWorldPos->SetFloat3((Ceng::FLOAT32*)&boundaryCenterWorldPos);

			program->fs_cameraPos->SetFloat3((Ceng::FLOAT32*)&cameraPos);
		}

	};

	class EnvMapManager : public Manager
	{
	protected:

		Ceng::RenderDevice* renderDevice;

		TextureManager* textureManager;
		ShaderManager* shaderManager;

		Ceng::SamplerState* diffuseSampler;

		std::vector<std::shared_ptr<CEngine::EnvProbeShader>> shaders;

		std::vector<std::shared_ptr<CEngine::EnvProbe>> probes;

		EnvMapManager();

	public:
		static EngineResult::value GetInstance(Ceng::RenderDevice* renderDevice, TextureManager* textureManager, 
			ShaderManager* shaderManager, EnvMapManager** output);

		~EnvMapManager() override;

		// Create simple environment shader from cubemap
		//EngineResult::value AddEnvMap(const Ceng::StringUtf8& cubemapFile);

		// Create cubemap with 
		EngineResult::value AddEnvMapParallaxAABB(const Ceng::StringUtf8& cubemapFile, Vec3 boundaryPos, Vec3 boxSideHalf);

		void Render(Ceng::RenderContext* renderContext, DeferredPassCommonParams* deferredParams, EnvMapCommonParams* envMapParams);
	};

}

#endif
