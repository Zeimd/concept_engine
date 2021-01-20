/*****************************************************************************
*
* material.h
*
* By Jari Korkala 3/11/2015
*
*****************************************************************************/

#ifndef CENGINE_MATERIAL_H
#define CENGINE_MATERIAL_H

#include <ceng.h>

#include "material-param.h"
#include "shader-program.h"

namespace CEngine
{
	class Material
	{
	public:
		std::shared_ptr<ShaderProgram> program;
		std::vector<std::unique_ptr<MaterialParam>> params;

		Ceng::ShaderConstant *vsFullTransform;
		Ceng::ShaderConstant *vsObjectRotation;

		Material();

	public:

		Material(const std::shared_ptr<ShaderProgram> &program, std::vector<std::unique_ptr<MaterialParam>> &&params,
			Ceng::ShaderConstant *vsFullTransform, Ceng::ShaderConstant *vsObjectRotation);

		virtual ~Material();

		virtual void Apply(Ceng::RenderContext *context,const Ceng::Matrix4 *fullTransform,
			const Ceng::Matrix4 *objectRotation,Ceng::SamplerState *textureSampler);
	};


}

#endif