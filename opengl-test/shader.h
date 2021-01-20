/*****************************************************************************
*
* shader.h
*
* By Jari Korkala 1/11/2015
*
*****************************************************************************/

#ifndef CENGINE_SHADER_DATA_H
#define CENGINE_SHADER_DATA_H

#include <ceng.h>

#include <unordered_map>

namespace CEngine
{
	namespace ShaderType
	{
		enum value
		{
			vertex,
			pixel,
			geometry,
		};
	}
	
	class Shader
	{
	public:
		ShaderType::value type;
		Ceng::StringUtf8 name;

		std::unordered_map<Ceng::StringUtf8,Ceng::BOOL> flags;

		virtual ~Shader()
		{
		}

		void AddFlags(const std::vector<Ceng::StringUtf8> *input)
		{
			if (input == nullptr) return;

			for (auto &x : *input)
			{
				flags[x] = true;
			}
		}

		virtual Ceng::VertexShader* AsVertexShader()
		{
			return nullptr;
		}

		virtual Ceng::PixelShader* AsPixelShader()
		{
			return nullptr;
		}

	};

	class VertexShader : public CEngine::Shader
	{
	public:
		Ceng::VertexShader *vshader;

		virtual ~VertexShader()
		{
			if (vshader != nullptr)
			{
				vshader->Release();
			}
		}

		virtual Ceng::VertexShader* AsVertexShader()
		{
			return vshader;
		}
	};

	class PixelShader : public CEngine::Shader
	{
	public:
		Ceng::PixelShader *pshader;

		virtual ~PixelShader()
		{
			if (pshader != nullptr)
			{
				pshader->Release();
			}
		}

		virtual Ceng::PixelShader* AsPixelShader()
		{
			return pshader;
		}

	};
}

#endif