/*****************************************************************************
*
* spot-light-shader.h
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#ifndef CENGINE_SPOT_LIGHT_SHADER_H
#define CENGINE_SPOT_LIGHT_SHADER_H

#include <memory>
#include <unordered_map>
#include "direction-light-shader.h"

namespace CEngine
{
	class ShaderManager;
	class Entity;
	class SpotLightComponent;
	class PointLightComponent;
	class RotationComponent;
	class PositionComponent;

	class SpotLightShader : public DirectionLightShader
	{
	public:
		Ceng::ShaderConstant *u_coneCosOuter;
		Ceng::ShaderConstant *u_coneCosInner;

	protected:
		SpotLightShader();
	public:
		SpotLightShader(ShaderManager &shaderManager);

		virtual ~SpotLightShader();

		void Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraFullTransform,
			const Ceng::Matrix4 *cameraRotation,
			PointLightComponent *light, RotationComponent *rotation,SpotLightComponent *spot,
			PositionComponent *position);
	};
}

#endif
