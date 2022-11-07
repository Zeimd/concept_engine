/*****************************************************************************
*
* gbuffer.h
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#ifndef CENGINE_GBUFFER_H
#define CENGINE_GBUFFER_H

#include <ceng.h>

#include "engine-result.h"

namespace CEngine
{
	class Gbuffer
	{
	public:

		Ceng::Texture2D* gbufferColor;
		Ceng::ShaderResourceView* gbufferColorView;
		Ceng::RenderTargetView* gbufferColorTarget;

		Ceng::Texture2D* gbufferNormal;
		Ceng::ShaderResourceView* gbufferNormalView;
		Ceng::RenderTargetView* gbufferNormalTarget;

		Ceng::Texture2D* depthTexture;
		Ceng::RenderTargetView* depthTarget;
		Ceng::ShaderResourceView* depthView;

		Ceng::Texture2D* hdrTexture;
		Ceng::ShaderResourceView* hdrView;
		Ceng::RenderTargetView* hdrTarget;


	protected:
		Gbuffer();

		Gbuffer(Ceng::Texture2D* gbufferColor, Ceng::ShaderResourceView* gbufferColorView, Ceng::RenderTargetView* gbufferColorTarget,
			Ceng::Texture2D* gbufferNormal, Ceng::ShaderResourceView* gbufferNormalView, Ceng::RenderTargetView* gbufferNormalTarget,
			Ceng::Texture2D* depthTexture, Ceng::RenderTargetView* depthTarget, Ceng::ShaderResourceView* depthView,
			Ceng::Texture2D* hdrTexture, Ceng::ShaderResourceView* hdrView, Ceng::RenderTargetView* hdrTarget);

	public:
		~Gbuffer();

		static EngineResult::value GetInstance(Ceng::RenderDevice* renderDevice, Ceng::UINT32 width, Ceng::UINT32 height, Gbuffer** output);
		
	};

}

#endif
