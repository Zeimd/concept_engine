/*****************************************************************************
*
* env-map-manager.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ENV_MAP_MANAGER_H
#define CENGINE_ENV_MAP_MANAGER_H

#include <memory>
#include <utility>

#include "manager.h"

#include "engine-result.h"

#include "render-params.h"

#include "texture.h"

namespace CEngine
{
	class TextureManager;
	class ShaderManager;

	class EnvProbeShader;
	class EnvProbe;

	class EnvMapEntry
	{
	public:
		std::shared_ptr<CEngine::EnvProbeShader> shader;
		std::vector<std::shared_ptr<CEngine::EnvProbe>> probes;
	};

	class EnvMapManager : public Manager
	{
	protected:

		Ceng::RenderDevice* renderDevice;

		TextureManager* textureManager;
		ShaderManager* shaderManager;

		Ceng::SamplerState* diffuseSampler;

		std::vector<EnvMapEntry> envMaps;

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
