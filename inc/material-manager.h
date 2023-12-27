/*****************************************************************************
*
* material-manager.h
*
* By Jari Korkala 27/5/2016
*
*****************************************************************************/

#ifndef CENGINE_MATERIAL_MANAGER_H
#define CENGINE_MATERIAL_MANAGER_H

#include <memory>
#include <unordered_map>

#include <ceng/datatypes/ce-string.h>

#include "engine-result.h"

#include "manager.h"

#include "json.h"

namespace CEngine
{
	class Material;

	class MaterialFile
	{
	public:
		json textureData;
		json materialData;

		std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Material>> materials;

	};
	
	class TextureManager;
	class ShaderManager;

	class MaterialManager : public Manager
	{
	protected:

		TextureManager *textureManager;
		ShaderManager *shaderManager;

		std::unordered_map<Ceng::StringUtf8,MaterialFile> files;	

		std::shared_ptr<Material> defaultMaterial;

	public:

		MaterialManager();
		~MaterialManager();

		MaterialManager(TextureManager *texManager, ShaderManager *shaderManager);

		const EngineResult::value LoadMaterialFile(const Ceng::StringUtf8 &file, Ceng::StringUtf8 &log);

		const EngineResult::value GetMaterial(const Ceng::StringUtf8 &file,const Ceng::StringUtf8 &name, 
			std::shared_ptr<Material> &out);

		const EngineResult::value CreateMaterial(MaterialFile &materialFile,
			const Ceng::StringUtf8 &name, json &data, std::shared_ptr<Material> &out);

	protected:
		const EngineResult::value SelectTexture(json &type,json::iterator &texIter);

	};
}

#endif
