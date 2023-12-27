/*****************************************************************************
*
* depth-pass.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_SHADOW_PASS_H
#define CENGINE_SHADOW_PASS_H

#include <memory>
#include <unordered_map>
#include <ceng.h>

namespace CEngine
{
	class Entity;
	class ShaderProgram;
	class ShaderManager;
	class ShadowmapComponent;

	class DepthPass
	{
	public:
		std::shared_ptr<ShaderProgram> program;

		Ceng::ShaderConstant *u_fullTransform;

		Ceng::RasterizerState rasterizerState;

		Ceng::DepthStencilState *depthState;

		DepthPass() = delete;
	public:
		
		DepthPass(Ceng::RenderDevice *renderDevice,ShaderManager &shaderManager);

		virtual ~DepthPass();

		void Render(Ceng::RenderContext *context,std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Entity>> &entityDict);

		void DepthRender(Ceng::RenderContext *renderContext, Ceng::Matrix4 &cameraFullTransform,
			Ceng::Matrix4 &projection,
			std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Entity>> &entityDict);
	};
}

#endif
