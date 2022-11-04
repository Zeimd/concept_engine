/*****************************************************************************
*
* env-probe.h
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#ifndef CENGINE_ENV_PROBE_H
#define CENGINE_ENV_PROBE_H

#include <memory>

#include <ceng.h>

#include "texture.h"
#include "math-vector.h"

#include "env-probe-shader.h"

namespace CEngine
{
	class EnvProbe
	{
	public:

		Ceng::StringUtf8 name;

		std::shared_ptr<CEngine::Texture> envMap;
		std::shared_ptr<CEngine::Texture> irradianceMap;

		Ceng::ShaderResourceView* envMapView;
		Ceng::ShaderResourceView* irradianceMapView;

	public:
		EnvProbe();

		virtual ~EnvProbe();

		virtual CEngine::EnvProbeShader* GetProgram();

		virtual void PrepareRender(const Vec3& cameraPos);
	};

	class EnvProbeAABOX : public EnvProbe
	{
	public:

		std::shared_ptr<CEngine::EnvProbeShaderParallaxAABB> program;

		Vec3 boxSideHalf;

		// Position where cubemap was generated
		Vec3 world_cubeGenPos;

	public:
		EnvProbeAABOX();

		~EnvProbeAABOX();

		CEngine::EnvProbeShader* GetProgram() override;

		void PrepareRender(const Vec3& cameraPos) override;
	};
}

#endif
