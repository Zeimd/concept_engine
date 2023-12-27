/*****************************************************************************
*
* img-loader.cpp
*
* By Jari Korkala 22/6/2016
*
* Loads images of common formats with DevIL library.
*
*****************************************************************************/

#include <FreeImage.h>
#include "texture-manager.h"

using namespace CEngine;

const EngineResult::value TextureManager::LoadTexture2D_FreeImage(const Ceng::StringUtf8 &filename,
	const TextureOptions &options, Ceng::Texture2D **out_texture)
{
	Ceng::StringUtf8 finalFile, title, extension;
	GetFileInfo(filename, finalFile, title, extension);

#ifdef _WIN32
	Ceng::StringUtf16 sysName = filename;
#endif

#ifdef _WIN32
	FREE_IMAGE_FORMAT fif =	FreeImage_GetFileTypeU(sysName.ToWString());
#else
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(filename.ToCString());
#endif

	if (fif == FIF_UNKNOWN)
	{
		return EngineResult::not_supported;
	}

#ifdef _WIN32
	FIBITMAP *image = FreeImage_LoadU(fif, sysName.ToWString());

#else
	FIBITMAP *image = FreeImage_Load(fif, filename.ToCString());
#endif

	if (image == nullptr)
	{
		return EngineResult::fail;
	}

	if (!FreeImage_HasPixels(image))
	{
		FreeImage_Unload(image);
		return EngineResult::not_supported;
	}

	unsigned int paletteSize = FreeImage_GetColorsUsed(image);

	if (paletteSize != 0)
	{
		FreeImage_Unload(image);
		return EngineResult::not_supported;
	}

	FREE_IMAGE_TYPE type = FreeImage_GetImageType(image);
	FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(image);

	unsigned int redMask = FreeImage_GetRedMask(image);
	unsigned int greenMask = FreeImage_GetGreenMask(image);
	unsigned int blueMask = FreeImage_GetBlueMask(image);

	Ceng::BOOL supported = true;

	switch (type)
	{
	case FIT_BITMAP:



		break;
	case FIT_UINT16:
		break;
	case FIT_UINT32:
		break;
	case FIT_FLOAT:
		break;
	case FIT_RGB16:
		break;
	case FIT_RGBA16:
		break;
	case FIT_RGBF:
		break;
	case FIT_RGBAF:
		break;
	default:
		supported = false;
	}

	if (!supported)
	{
		FreeImage_Unload(image);
		return EngineResult::not_supported;
	}

	if (colorType == FIC_RGB)
	{


	}
	else if (colorType == FIC_RGBALPHA)
	{
	}
	else
	{
		FreeImage_Unload(image);
		return EngineResult::not_supported;
	}
	
	Bitmap bitmap;

	bitmap.width = FreeImage_GetWidth(image);
	bitmap.height = FreeImage_GetHeight(image);
	bitmap.bytesPerPixel = FreeImage_GetBPP(image) / 8;

	bitmap.pitch = FreeImage_GetLine(image);
	
	FreeImage_Unload(image);
	return EngineResult::ok;
}
