/*****************************************************************************
*
* shader-manager.h
*
* By Jari Korkala 1/11/2015
*
*****************************************************************************/

#ifndef ENGINE_SHADER_MANAGER_H
#define ENGINE_SHADER_MANAGER_H

#include <list>

#include <ceng.h>

#include "json.h"

#include "shader.h"

#include "engine-result.h"

#include "manager.h"

namespace CEngine
{
	class ShaderProgram;

	class MaterialShader;
	
	class ShaderManager : public Manager
	{
	protected:

		Ceng::RenderDevice *renderDevice;

		std::list<std::shared_ptr<Shader>> shaderList;

		std::list<std::shared_ptr<ShaderProgram>> programList;

		ShaderManager();
	public:

		ShaderManager(Ceng::RenderDevice *renderDevice);
	
		~ShaderManager();

		void PruneShaderList();

		void PruneProgramList();

		const EngineResult::value LoadShader(const ShaderType::value type, const Ceng::StringUtf8 &file,
			const std::vector<Ceng::StringUtf8> *flags,
			std::shared_ptr<Shader> &output, Ceng::StringUtf8 &log);

		const std::shared_ptr<Shader> FindShader(const ShaderType::value type, const Ceng::StringUtf8 &file,
			const std::vector<Ceng::StringUtf8> *flags);

		const EngineResult::value ShaderManager::CreateProgramFromFile(const Ceng::StringUtf8 &vshaderFile,
			const std::vector<Ceng::StringUtf8> *vsFlags,
			const Ceng::StringUtf8 &pshaderFile, const std::vector<Ceng::StringUtf8> *psFlags,
			std::shared_ptr<CEngine::ShaderProgram> &program);

		const EngineResult::value ShaderManager::CreateProgramFromFile(const Ceng::StringUtf8 &vshaderFile,
			const Ceng::StringUtf8 &pshaderFile,
			std::shared_ptr<CEngine::ShaderProgram> &program);

		const std::shared_ptr<ShaderProgram> FindProgram(std::shared_ptr<Shader> &vertexShader, std::shared_ptr<Shader> &pixelShader);

		//const std::shared_ptr<MaterialShader> FindMaterialShader(const Ceng::StringUtf8 &file);
	};
}

#endif