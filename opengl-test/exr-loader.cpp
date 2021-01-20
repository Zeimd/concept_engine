/*****************************************************************************
*
* exr-loader.cpp
*
* Created By Jari Korkala 22/6/2016
*
*****************************************************************************/


#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>
#include "texture-manager.h"

using namespace CEngine;

const Ceng::UINT32 BytesPerChannel(const int pixelType)
{
	switch (pixelType)
	{
	case TINYEXR_PIXELTYPE_FLOAT:
	case TINYEXR_PIXELTYPE_UINT:
		return 4;
	case TINYEXR_PIXELTYPE_HALF:
		return 2;
	default:
		break;
	}

	return 0;
}

const EngineResult::value TextureManager::LoadBitmap_EXR(const Ceng::StringUtf8 &filename, const TextureOptions &options,
	CEngine::Bitmap &bitmap)
{
	Ceng::StringUtf8 text = "EXR Loader : \n";
	text += filename + "\n";

	Ceng::Log::Print(text);

	const char *errorText;

	EXRImage image;

	InitEXRImage(&image);

	int result = ParseMultiChannelEXRHeaderFromFile(&image, filename.ToCString(), &errorText);

	if (result != 0)
	{
		text = "Error : ";
		text += errorText;
		text += "\n";

		Ceng::Log::Print(text);
		FreeEXRImage(&image);
		return EngineResult::fail;
	}

	result = LoadMultiChannelEXRFromFile(&image, filename.ToCString(), &errorText);

	if (result != 0)
	{
		text = "Error : ";
		text += errorText;
		text += "\n";

		Ceng::Log::Print(text);
		FreeEXRImage(&image);
		return EngineResult::fail;
	}

	int redType, greenType, blueType, alphaType;
	Ceng::INT32 redChannel = -1, greenChannel = -1, blueChannel = -1, alphaChannel = -1;
	Ceng::INT32 redBytes, greenBytes, blueBytes, alphaBytes;

	for (Ceng::UINT32 k = 0; k < image.num_channels; ++k)
	{
		Ceng::StringUtf8 name = image.channel_names[k];

		if (name == "R")
		{
			redType = image.pixel_types[k];
			redChannel = k;
			redBytes = BytesPerChannel(image.pixel_types[k]);
		}
		else if (name == "G")
		{
			greenType = image.pixel_types[k];
			greenChannel = k;
			greenBytes = BytesPerChannel(image.pixel_types[k]);
		}
		else if (name == "B")
		{
			blueType = image.pixel_types[k];
			blueChannel = k;
			blueBytes = BytesPerChannel(image.pixel_types[k]);
		}
		else if (name == "A")
		{
			alphaType = image.pixel_types[k];
			alphaChannel = k;
			alphaBytes = BytesPerChannel(image.pixel_types[k]);
		}
		else
		{
			text = "Warning : unrecognized channel : \"";
			text += image.channel_names[k];
			text += "\"\n";

			Ceng::Log::Print(text);
		}
	}

	if (redChannel == -1 || redBytes == 0)
	{
		Ceng::Log::Print("Error : image doesn't have red channel");
		FreeEXRImage(&image);
		return EngineResult::not_supported;
	}

	if (greenChannel == -1 || greenBytes == 0)
	{
		Ceng::Log::Print("Error : image doesn't have green channel");
		FreeEXRImage(&image);
		return EngineResult::not_supported;
	}

	if (blueChannel == -1 || blueBytes == 0)
	{
		Ceng::Log::Print("Error : image doesn't have blue channel");
		FreeEXRImage(&image);
		return EngineResult::not_supported;
	}

	// Check that all channels have same type

	if (greenType != redType)
	{
		Ceng::Log::Print("Error : channel type mismatch : red-green");
		FreeEXRImage(&image);
		return EngineResult::not_supported;
	}
	if (blueType != redType)
	{
		Ceng::Log::Print("Error : channel type mismatch : red-blue");
		FreeEXRImage(&image);
		return EngineResult::not_supported;
	}
	if (alphaChannel > 0 && alphaType != redType)
	{
		Ceng::Log::Print("Error : channel type mismatch : red-alpha");
		FreeEXRImage(&image);
		return EngineResult::not_supported;
	}

	Ceng::IMAGE_FORMAT::value format;

	// Determine format
	if (redType == TINYEXR_PIXELTYPE_FLOAT)
	{
		format = Ceng::IMAGE_FORMAT::cf32_bgr;
		if (alphaChannel != -1)
		{
			format = Ceng::IMAGE_FORMAT::CF32_ABGR;
		}
	}
	else if (redType == TINYEXR_PIXELTYPE_HALF)
	{
		if (alphaChannel < 0)
		{
			alphaBytes = 2;
			/*
			Ceng::Log::Print("Error : half-precision not supported without alpha channel\n");
			FreeEXRImage(&image);
			return EngineResult::not_supported;
			*/
		}
		format = Ceng::IMAGE_FORMAT::CF16_ABGR;
	}
	else if (redType == TINYEXR_PIXELTYPE_UINT)
	{
		format = Ceng::IMAGE_FORMAT::uint_b32_g32_r32;
		if (alphaChannel != -1)
		{
			format = Ceng::IMAGE_FORMAT::uint_a32_b32_g32_r32;
		}
	}

	Ceng::UINT32 bytesPerPixel = redBytes + greenBytes + blueBytes + alphaBytes;

	Ceng::UINT32 imageSize = image.width * image.height * bytesPerPixel;
	Ceng::UINT32 pitch = image.width * bytesPerPixel;

	bitmap.width = image.width;
	bitmap.height = image.height;
	bitmap.format = format;

	bitmap.bytesPerPixel = bytesPerPixel;

	bitmap.imageSize = imageSize;
	bitmap.pitch = pitch;

	// Allocate buffer and construct bitmap

	bitmap.imageBuffer = std::vector<Ceng::UINT8>(imageSize);

	Ceng::UINT8 *red = image.images[redChannel];
	Ceng::UINT8 *green = image.images[greenChannel];
	Ceng::UINT8 *blue = image.images[blueChannel];

	for (Ceng::UINT32 y = 0; y < image.height; ++y)
	{
		for (Ceng::UINT32 x = 0; x < image.width; ++x)
		{
			Ceng::UINT32 destOffset = y*pitch + x*bytesPerPixel;

			Ceng::UINT32 sourceOffset = y*image.width*redBytes + x*redBytes;

			memcpy(&bitmap.imageBuffer[destOffset], &red[sourceOffset], redBytes);

			memcpy(&bitmap.imageBuffer[destOffset + redBytes], &green[sourceOffset], redBytes);

			memcpy(&bitmap.imageBuffer[destOffset + 2 * redBytes], &blue[sourceOffset], redBytes);
		}
	}

	if (alphaChannel != -1)
	{
		Ceng::UINT8 *alpha = image.images[alphaChannel];

		for (Ceng::UINT32 y = 0; y < image.height; ++y)
		{
			for (Ceng::UINT32 x = 0; x < image.width; ++x)
			{
				Ceng::UINT32 destOffset = y*pitch + x*bytesPerPixel;

				Ceng::UINT32 sourceOffset = y*image.width*redBytes + x*redBytes;

				memcpy(&bitmap.imageBuffer[destOffset + 3 * redBytes], &alpha[sourceOffset], redBytes);
			}
		}
	}
	else if (format == Ceng::IMAGE_FORMAT::CF16_ABGR)
	{
		Ceng::UINT16 alphaValue = 0;

		for (Ceng::UINT32 y = 0; y < image.height; ++y)
		{
			for (Ceng::UINT32 x = 0; x < image.width; ++x)
			{
				Ceng::UINT32 destOffset = y*pitch + x*bytesPerPixel;

				memcpy(&bitmap.imageBuffer[destOffset + 3 * redBytes], &alphaValue, redBytes);
			}
		}
	}

	FreeEXRImage(&image);

	return EngineResult::ok;
}

const EngineResult::value TextureManager::LoadTexture2D_EXR(const Ceng::StringUtf8 &filename,
	const TextureOptions &options, Ceng::Texture2D **out_texture)
{
	EngineResult::value eresult;

	Bitmap bitmap;

	eresult = LoadBitmap_EXR(filename, options, bitmap);
	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	// Configure descriptor

	Ceng::Texture2dDesc desc;

	desc.width = bitmap.width;
	desc.height = bitmap.height;

	desc.format = bitmap.format;

	desc.sRGB = false;
	desc.mipLevels = 0;
	desc.arraySize = 0;
	desc.usage = options.usage;
	desc.bindFlags = options.bindFlags;
	desc.cpuAccessFlags = options.cpuAccessFlags;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;

	desc.optionFlags = options.options;

	Ceng::SubResourceData resource;

	resource.sourcePtr = &bitmap.imageBuffer[0];

	resource.rowPitch = bitmap.pitch;
	resource.depthPitch = bitmap.imageSize;

	Ceng::CRESULT cresult = renderDevice->CreateTexture2D(desc, &resource, out_texture);
	
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}
	
	return EngineResult::ok;
}

const EngineResult::value TextureManager::LoadCubemap_EXR(const Ceng::StringUtf8 &filename,
	const TextureOptions &options, Ceng::Cubemap **out_texture)
{
	return EngineResult::ok;
}