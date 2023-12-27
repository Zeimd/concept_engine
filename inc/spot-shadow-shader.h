/*****************************************************************************
*
* spot-shadow-shader.h
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#ifndef CENGINE_SPOT_SHADOW_SHADER_H
#define CENGINE_SPOT_SHADOW_SHADER_H

#include "spot-light-shader.h"

namespace CEngine
{
	class ShadowmapComponent;

	class SpotShadowShader : public SpotLightShader
	{
	public:

		Ceng::ShaderConstant *u_shadowMap;

		Ceng::ShaderConstant *u_lightProjection;

		Ceng::ShaderConstant *u_cameraTransform;

		Ceng::ShaderConstant *u_depthBias;

	protected:
		SpotShadowShader();
	public:

		SpotShadowShader(ShaderManager &shaderManager);
		virtual ~SpotShadowShader();

		void Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraFullTransform,
			const Ceng::Matrix4 *cameraRotation,
			PointLightComponent *light, RotationComponent *rotation, SpotLightComponent *spot,
			PositionComponent *position,ShadowmapComponent *shadow,Ceng::SamplerState *shadowSampler);
	};
}

#endif
