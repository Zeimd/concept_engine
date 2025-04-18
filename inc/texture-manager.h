/*****************************************************************************
*
* texture-manager.h
*
* By Jari Korkala 3/11/2015
*
*****************************************************************************/

#ifndef CENGINE_TEXTURE_MANAGER_H
#define CENGINE_TEXTURE_MANAGER_H

#include <vector>
#include <unordered_map>
#include <list>

#include "json.h"

#include <ceng.h>

#include "texture.h"

#include "engine-result.h"

#include "manager.h"

#include "texture-options.h"

namespace CEngine
{
	class TextureManager : public Manager
	{
	protected:

		Ceng::RenderDevice *renderDevice;

		std::list<std::shared_ptr<Texture>> textureList;

		std::shared_ptr<Texture> missingMaterialData2D;

		std::shared_ptr<Texture> missingDiffuse2D;
		std::shared_ptr<Texture> missingNormal2D;
		std::shared_ptr<Texture> missingMaterial2D;
		std::shared_ptr<Texture> missingMaterialFile2D;

		TextureManager();
	public:
		TextureManager(Ceng::RenderDevice *renderDevice);
		
		virtual ~TextureManager();

		void UnloadAll();

		/*
		// Get handle to a texture resource. It is loaded if not present in memory.
		const std::size_t Find(const Ceng::StringUtf8 &filename,
			TextureType::value type, const Ceng::BOOL sRGB);

		// Get actual resource by handle. Locks resource until released.
		const std::shared_ptr<Texture> Lock(const std::size_t handle);

		void Unlock(const std::size_t handle);
		*/

		const EngineResult::value PeekAndValidateFile(const json &filename, const TextureType::value type,
			const Ceng::BOOL sRGB,Ceng::IMAGE_FORMAT::value &out_format);

		const std::shared_ptr<Texture> FindTexture(const Ceng::StringUtf8 &filename,
			TextureType::value type, const Ceng::BOOL sRGB);

		const EngineResult::value LoadTexture2D(const Ceng::StringUtf8 &filename,
			const TextureOptions &options,std::shared_ptr<Texture> &output);

		// Loads a cubemap. 
		//
		// It is possible to load from six separate image files.
		// In this case *filename* shall give full name of the image file sequence EXCEPT for numbering:
		// 
		// fileName = "img.exr"
		// expected sequence:
		//     img_0.exr, img_1.exr,...
		const EngineResult::value LoadCubemap(const Ceng::StringUtf8 &filename,
			const TextureOptions &options, std::shared_ptr<Texture> &output, std::shared_ptr<Texture> &out_irradiance);

	protected:

		const EngineResult::value LoadBitmap_EXR(const Ceng::StringUtf8 &filename, 
			const TextureOptions &options,CEngine::Bitmap &bitmap);

		const EngineResult::value LoadBitmap_BMP(const Ceng::StringUtf8 &filename, 
			const TextureOptions &options, Bitmap &output);

		const EngineResult::value LoadTexture2D_BMP(const Ceng::StringUtf8 &filename, 
			const TextureOptions &options, Ceng::Texture2D **out_texture);

		const EngineResult::value LoadTexture2D_PNG(const Ceng::StringUtf8 &filename,
			const TextureOptions &options, Ceng::Texture2D **out_texture);

		const EngineResult::value LoadTexture2D_FreeImage(const Ceng::StringUtf8 &filename,
			const TextureOptions &options, Ceng::Texture2D **out_texture);

		const EngineResult::value LoadTexture2D_EXR(const Ceng::StringUtf8 &filename,
			const TextureOptions &options, Ceng::Texture2D **out_texture);

		const EngineResult::value LoadTexture2D_KTX(const Ceng::StringUtf8 &filename, 
			const TextureOptions &options, Ceng::Texture2D **out_texture);

		const EngineResult::value LoadTexture2D_DDS(const Ceng::StringUtf8 &filename, 
			const TextureOptions &options,Ceng::Texture2D **out_texture);

		const EngineResult::value LoadCubemap_BMP(const Ceng::StringUtf8 &filename,
			const TextureOptions &options, Ceng::Cubemap **out_texture);

		const EngineResult::value LoadCubemap_EXR(const Ceng::StringUtf8 &filename,
			const TextureOptions &options, Ceng::Cubemap **out_texture);

		const EngineResult::value GenerateIrradianceMapFromCube(const Ceng::StringUtf8& name, 
			const TextureOptions& options, Ceng::Cubemap* cubemap, Ceng::Cubemap** irradianceMap);

		const EngineResult::value LoadCubemapFromSequence(const Ceng::StringUtf8 &filename,
			const Ceng::StringUtf8 &extension,
			const TextureOptions &options, Ceng::Cubemap **out_texture);
	};
}

#endif