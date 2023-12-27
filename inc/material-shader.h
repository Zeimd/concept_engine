/*****************************************************************************
*
* material-shader.h
*
* By Jari Korkala 26/5/2016
*
*****************************************************************************/

#ifndef CENGINE_MATERIAL_SHADER_H
#define CENGINE_MATERIAL_SHADER_H

#include <memory>

#include <ceng/datatypes/ce-string.h>

namespace CEngine
{
	class ShaderProgram;

	namespace ParamType
	{
		enum value
		{
			float1 ,
			float2 ,
			float3 ,
			float4 ,
			tex_1d ,
			tex_2d ,
			tex_cubemap ,
		};
	};

	class AbstractValue
	{

	};

	class ParamDef
	{
	public:
		Ceng::StringUtf8 name;

		AbstractType *defaultValue;
		AbstractType *minValue;
		AbstractType *maxValue;
	};

	class MaterialShader
	{
	public:
		Ceng::StringUtf8 name;

		std::shared_ptr<ShaderProgram> program;



	protected:

		MaterialShader();
	public:

		static std::shared_ptr<MaterialShader> GetInstance();

		~MaterialShader();


	};
}

#endif
