/*****************************************************************************
*
* point-light-shader.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_POINT_LIGHT_SHADER_H
#define CENGINE_POINT_LIGHT_SHADER_H

#include <memory>
#include <ceng.h>

namespace CEngine
{
	class ShaderManager;
	class ShaderProgram;
	class PointLightComponent;

	class PointLightShader
	{
	public:
		std::shared_ptr<ShaderProgram> program;

		Ceng::ShaderConstant *u_windowWidth;
		Ceng::ShaderConstant *u_windowHeight;

		Ceng::ShaderConstant *u_xDilationDiv;
		Ceng::ShaderConstant *u_yDilationDiv;

		Ceng::ShaderConstant *u_zTermA;
		Ceng::ShaderConstant *u_zTermB;

		Ceng::ShaderConstant *u_gbufferColor;
		Ceng::ShaderConstant *u_gbufferNormal;

		Ceng::ShaderConstant *u_depthBuffer;
		Ceng::ShaderConstant *u_lightPos;

		Ceng::ShaderConstant *u_lightPower;
		Ceng::ShaderConstant *u_lightCutoff;

	protected:
		PointLightShader();
	public:
		

		PointLightShader(ShaderManager &program);

		void InitCommonUniforms();

		virtual ~PointLightShader();

		/**
		 * Sets light source independent parameters.
		 */
		void Configure(Ceng::RenderContext *context, Ceng::UINT32 displayWidth, const Ceng::UINT32 displayHeight,
			const Ceng::FLOAT32 xDilationDiv, const Ceng::FLOAT32 yDilationDiv,
			const Ceng::FLOAT32 zTermA, const Ceng::FLOAT32 zTermB, const Ceng::INT32 colorBufferTex,
			const Ceng::INT32 normalBufferTex, const Ceng::INT32 depthBufferTex);

		void Render(Ceng::RenderContext *context,const Ceng::Matrix4 *cameraFullTransform,
			PointLightComponent *light);
	};
}

#endif
