/*****************************************************************************
*
* direction-light-shader.h
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#ifndef CENGINE_DIRECTION_LIGHT_SHADER_H
#define CENGINE_DIRECTION_LIGHT_SHADER_H

#include "point-light-shader.h"

namespace CEngine
{
	class PointLightComponent;
	class ShaderManager;
	class RotationComponent;

	class DirectionLightShader : public PointLightShader
	{
	public:

		Ceng::ShaderConstant *u_lightDir;

	protected:
		DirectionLightShader();
	public:
		DirectionLightShader(ShaderManager &shaderManager);

		virtual ~DirectionLightShader();

		void Render(Ceng::RenderContext *context, const Ceng::Matrix4 *cameraRotation,
			PointLightComponent *light,RotationComponent *rotation);
	};
}

#endif
