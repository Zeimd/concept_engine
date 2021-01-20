/*****************************************************************************
*
* shader-program.h
*
* By Jari Korkala 2/11/2015
*
*****************************************************************************/

#ifndef CENGINE_SHADER_PROGRAM_H
#define CENGINE_SHADER_PROGRAM_H

#include <ceng.h>

namespace CEngine
{
	class Shader;

	class ShaderProgram
	{
	public:

		std::shared_ptr<Shader> vertexShader;
		std::shared_ptr<Shader> pixelShader;

		Ceng::ShaderProgram *program;

		Ceng::StringUtf8 managerLog;

	public:
		ShaderProgram()
		{
		}

		~ShaderProgram()
		{
			if (program != nullptr)
			{
				program->Release();
			}
		}

		Ceng::ShaderProgram* GetProgram()
		{
			return program;
		}


	};
}

#endif