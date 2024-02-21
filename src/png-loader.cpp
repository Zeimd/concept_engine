/*****************************************************************************
*
* png-loader.h
*
* By Jari Korkala 16/6/2016
*
* Uses uPNG library
*
*****************************************************************************/

#include <upng.h>

#include "texture-manager.h"

using namespace CEngine;
const EngineResult::value TextureManager::LoadTexture2D_PNG(const Ceng::StringUtf8 &filename,
	const TextureOptions &options, Ceng::Texture2D **out_texture)
{
	upng_t *png;
	upng_error pngError;

	png = upng_new_from_file(filename.ToCString());
	if (png != nullptr) {
		pngError = upng_decode(png);
		if (pngError != UPNG_EOK)
		{
			upng_free(png);

			switch (pngError)
			{
			case UPNG_ENOMEM:
				return EngineResult::out_of_memory;
			case UPNG_ENOTFOUND:
				return EngineResult::file_not_found;
			case UPNG_ENOTPNG :
				return EngineResult::not_supported;
			case UPNG_EMALFORMED:
			case UPNG_EUNSUPPORTED:
			case UPNG_EUNINTERLACED:
			case UPNG_EUNFORMAT:
				return EngineResult::not_supported;
			case UPNG_EPARAM:
				return EngineResult::invalid_param;
			default:
				return EngineResult::fail;
			}
		}
	}
	else
	{
		return EngineResult::fail;
	}

	Bitmap bitmap;

	bitmap.width = upng_get_width(png);
	bitmap.height = upng_get_height(png);

	bitmap.imageSize = upng_get_size(png);

	bitmap.imageBuffer = std::vector<Ceng::UINT8>(bitmap.imageSize);

	const Ceng::UINT8 *sourcePtr = upng_get_buffer(png);

	// Swap blue and red channels

	Ceng::INT32 format = upng_get_format(png);

	switch (format)
	{
	case UPNG_RGB8:
		bitmap.format = Ceng::IMAGE_FORMAT::unorm_b8_g8_r8;
		bitmap.bytesPerPixel = 3;
		bitmap.pitch = bitmap.width*bitmap.bytesPerPixel;

		memcpy(&bitmap.imageBuffer[0], sourcePtr, bitmap.pitch*bitmap.height);
		/*
		for (Ceng::UINT32 y = 0; y < bitmap.height; ++y)
		{
			for (Ceng::UINT32 x = 0; x < bitmap.width; ++x)
			{
				Ceng::UINT32 offset = y*bitmap.pitch + x*bitmap.bytesPerPixel;

				bitmap.imageBuffer[offset] = sourcePtr[offset];
				bitmap.imageBuffer[offset + 1] = sourcePtr[offset + 1];
				bitmap.imageBuffer[offset + 2] = sourcePtr[offset+2];

				//bitmap.imageBuffer[offset] = sourcePtr[offset + 2];
				//bitmap.imageBuffer[offset + 1] = sourcePtr[offset + 1];
				//bitmap.imageBuffer[offset + 2] = sourcePtr[offset];
			}
		}
		*/
		break;
	case UPNG_RGBA8:
		bitmap.format = Ceng::IMAGE_FORMAT::unorm_a8_b8_g8_r8;
		bitmap.bytesPerPixel = 4;
		bitmap.pitch = bitmap.width*bitmap.bytesPerPixel;

		memcpy(&bitmap.imageBuffer[0], sourcePtr, bitmap.pitch*bitmap.height);

		/*
		for (Ceng::UINT32 y = 0; y < bitmap.height; ++y)
		{
			for (Ceng::UINT32 x = 0; x < bitmap.width; ++x)
			{
				Ceng::UINT32 offset = y*bitmap.pitch + x*bitmap.bytesPerPixel;

				bitmap.imageBuffer[offset] = sourcePtr[offset];
				bitmap.imageBuffer[offset + 1] = sourcePtr[offset + 1];
				bitmap.imageBuffer[offset + 2] = sourcePtr[offset + 2];
				
				bitmap.imageBuffer[offset] = sourcePtr[offset + 2];
				bitmap.imageBuffer[offset + 1] = sourcePtr[offset + 1];
				bitmap.imageBuffer[offset + 2] = sourcePtr[offset];
				
				bitmap.imageBuffer[offset + 3] = sourcePtr[offset + 3];
			}
		}
*/
		break;
	default:
		upng_free(png);
		return EngineResult::not_supported;
	};


	upng_free(png);

	bitmap.sRGB = false;

	Ceng::Texture2dDesc desc;

	// Configure descriptor

	desc.width = bitmap.width;
	desc.height = bitmap.height;
	desc.format = bitmap.format;
	desc.sRGB = options.sRGB;
	desc.mipLevels = 0;
	desc.arraySize = 0;
	desc.usage = options.usage;
	desc.bindFlags = options.bindFlags;
	desc.cpuAccessFlags = options.cpuAccessFlags;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;

	desc.optionFlags = options.options;

	Ceng::SubResourceData image;

	image.sourcePtr = &bitmap.imageBuffer[0];
	image.rowPitch = bitmap.pitch;
	image.depthPitch = bitmap.imageSize;

	Ceng::CRESULT cresult = renderDevice->CreateTexture2D(desc, &image, out_texture);

	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	return EngineResult::ok;
}
