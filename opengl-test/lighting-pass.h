/*****************************************************************************
*
* lighting-pass.h
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#ifndef CENGINE_LIGHTING_PASS_H
#define CENGINE_LIGHTING_PASS_H

#include <ceng.h>

#include "render-params.h"
#include "engine-result.h"

namespace CEngine
{
	class FullScreenQuad;
	class Gbuffer;
	class EnvMapManager;
	class Camera;

	class LightingPass
	{
	public:


		Ceng::BlendState* lightBlendState;

		Ceng::DepthStencilState* lightDepthState;

		std::shared_ptr<EnvMapManager> envMapManager;

	protected:

		LightingPass();

	public:

		LightingPass(Ceng::BlendState* lightBlendState, Ceng::DepthStencilState* lightDepthState,
			std::shared_ptr<EnvMapManager>& envMapManager);

		~LightingPass();

		static EngineResult::value GetInstance(Ceng::RenderDevice* renderDevice, std::shared_ptr<EnvMapManager>& envMapManager,
			LightingPass** output);

		void Render(Ceng::RenderContext* renderContext, Gbuffer* gbuffer, FullScreenQuad* quad,
			Ceng::SamplerState* nearestSampler, Camera* camera,
			DeferredPassCommonParams* deferredParams, EnvMapCommonParams* envMapParams);
	};
}

#endif
