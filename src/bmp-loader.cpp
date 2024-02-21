/*****************************************************************************
*
* bmp-loader.cpp
*
* Created By Jari Korkala 30/10/2015
*
*****************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

#include "byte-order.h"
#include "texture-manager.h"

using namespace CEngine;

const EngineResult::value TextureManager::LoadBitmap_BMP(const Ceng::StringUtf8 &filename,
	const CEngine::TextureOptions &options, Bitmap &output)
{
	FILE *stream;
	Ceng::UINT8 buffer[16];
	Ceng::INT32 k;
	Ceng::BOOL windowsbitmap = false;

	Ceng::UINT32 filesize = 0;
	Ceng::UINT32 bitmapoffset = 0;
	Ceng::UINT32 bitmapsize = 0;
	Ceng::UINT32 padding = 0;

	Ceng::UINT32 headersize = 0;
	Ceng::UINT32 compressionmethod = 0;
	Ceng::UINT32 importantcolors = 0;
	Ceng::UINT16 colorplanes = 0;
	Ceng::UINT16 colordepthBits = 0;
	Ceng::UINT32 paletteSize = 0;

	stream = fopen(filename.ToCString(), "rb");
	if (stream == NULL)
	{
		return EngineResult::file_not_found;
	}

	fread(&buffer[0], 1, 2, stream); // Read identification data
	if (feof(stream))
	{
		fclose(stream);
		return EngineResult::io_error;
	}

	// Check that the file is a bitmap
	if (buffer[0] != 'B' && buffer[1] != 'M')
	{
		fclose(stream);
		return EngineResult::io_error;
	}

	// Read bitmap header
	fread(&filesize, 4, 1, stream);
	fread(&buffer[0], 4, 1, stream); // Skip unused data
	fread(&bitmapoffset, 4, 1, stream);

	fread(&headersize, 4, 1, stream);

	// Read bitmap info
	switch (headersize)
	{
	case 12: // OS/2 header
		fclose(stream);
		return EngineResult::invalid_texture_type;
		break;
	case 40: // Windows bitmap version 3
		fread(&output.width, 4, 1, stream);
		fread(&output.height, 4, 1, stream);
		fread(&colorplanes, 2, 1, stream); // Color plane counter not used so skip it			
		fread(&colordepthBits, 2, 1, stream); // Read color depth

		fread(&compressionmethod, 4, 1, stream);
		fread(&bitmapsize, 4, 1, stream);
		fread(&buffer[0], 4, 2, stream); // Skip physical resolution data
		fread(&paletteSize, 4, 1, stream);
		fread(&importantcolors, 4, 1, stream); // Skip important colors

		if (compressionmethod != 0)
		{
			fclose(stream);
			return EngineResult::invalid_texture_type;
		}

		windowsbitmap = true;
		break;
	case 64:	// OS/2 v2
		fclose(stream);
		return EngineResult::invalid_texture_type;
		break;
	case 108: // Windows version 4
		fclose(stream);
		windowsbitmap = true;
		return EngineResult::invalid_texture_type;
		break;
	case 124: // Windows version 5
		fclose(stream);
		windowsbitmap = true;
		return EngineResult::invalid_texture_type;
		break;
	default: // Unknown header type
		fclose(stream);
		return EngineResult::invalid_texture_type;
	}

	Ceng::UINT32 bytesPerPixel = colordepthBits / 8;

	switch (bytesPerPixel)
	{
	case 1:
		fclose(stream);
		return EngineResult::invalid_texture_type;
	case 2:
		output.format = Ceng::IMAGE_FORMAT::unorm_a1_r5_g5_b5;
		//colorFormat = COLOR_A1R5G5B5;
		break;
	case 3:
		output.format = Ceng::IMAGE_FORMAT::unorm_r8_g8_b8;
		break;
	case 4:
		output.format = Ceng::IMAGE_FORMAT::unorm_a8_r8_g8_b8;
		break;
	default:
		fclose(stream);
		return EngineResult::invalid_texture_type;
		break;
	}



	// Calculate correct image size if not specified in the header
	//if (bitmapsize == 0) 
	//{
	output.bytesPerPixel = bytesPerPixel;
	output.pitch = output.width*bytesPerPixel;
	output.imageSize = output.width*output.height*bytesPerPixel;
	//}

	// Allocate space for the bitmap

	output.imageBuffer = std::vector<Ceng::UINT8>(output.imageSize);

	/*
	usekey = 0;
	colorkey[0] = 0;
	colorkey[1] = 0;
	colorkey[2] = 0;
	colorkey[3] = 0;

	// Read palette if it exists
	if (colorFormat == COLOR_PAL256 && palettesize > 0)
	{
	palette = (PALETTEENTRY*)malloc(palettesize*sizeof(PALETTEENTRY));
	if (palette == NULL)
	{
	fclose(stream);
	free(bitmap);
	return 0;
	}
	for (k = 0; k<palettesize; k++)
	{
	fread(&palette[k].peBlue, 1, 1, stream);
	fread(&palette[k].peGreen, 1, 1, stream);
	fread(&palette[k].peRed, 1, 1, stream);
	if (windowsbitmap == true) fread(&buffer[0], 1, 1, stream); // Skip the last byte
	palette[k].peFlags = 0;
	}
	}
	else
	{
	palettesize = 0;
	}
	*/

	padding = (output.width*bytesPerPixel) % 4;
	if (padding > 0)
	{
		padding = 4 - padding;
	}

	Ceng::UINT32 offset;

	// Read the bitmap
	fseek(stream, bitmapoffset, SEEK_SET);

	Ceng::UINT32 rowPitch = output.width*bytesPerPixel;

	//offset = bitmapsize - rowPitch;
	offset = output.imageSize - rowPitch;
	for (k = output.height; k>0; k--)
	{
		fread(&output.imageBuffer[offset], 1, rowPitch, stream);
		fread(&buffer[0], 1, padding, stream);
		offset -= rowPitch;
	}

	fclose(stream);

	return EngineResult::ok;
}

const EngineResult::value TextureManager::LoadTexture2D_BMP(const Ceng::StringUtf8 &filename, 
	const CEngine::TextureOptions &options,Ceng::Texture2D **out_texture)
{
	
	EngineResult::value eresult;

	Bitmap bitmap;

	eresult = LoadBitmap_BMP(filename, options, bitmap);
	if (eresult != EngineResult::ok)
	{
		return eresult;
	}
	
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

const EngineResult::value TextureManager::LoadCubemap_BMP(const Ceng::StringUtf8 &filename,
	const CEngine::TextureOptions &options, Ceng::Cubemap **out_texture)
{
	auto bitmaps = std::vector<Bitmap>(6);

	auto iter = filename.FindLast('.', filename.ConstBeginIterator());

	Ceng::StringUtf8 name = filename.SubString(filename.ConstBeginIterator(), iter);

	++iter;

	Ceng::StringUtf8 extension = filename.SubString(iter, filename.ConstEndIterator());

	// Load first image

	EngineResult::value eresult;

	eresult = LoadBitmap_BMP(filename, options, bitmaps[0]);
	if (eresult != EngineResult::ok)
	{
		return eresult;
	}

	// Load remaining

	for (Ceng::UINT32 k = 1; k < 6; ++k)
	{
		Ceng::StringUtf8 chainName = name + k + '.' + extension;
		//Ceng::Log::Print(chainName);

		eresult = LoadBitmap_BMP(chainName, options, bitmaps[k]);
		if (eresult != EngineResult::ok)
		{
			return eresult;
		}

		if (bitmaps[k].width != bitmaps[0].width ||
			bitmaps[k].height != bitmaps[0].height ||
			bitmaps[k].format != bitmaps[0].format)
		{
			return EngineResult::texture_type_mismatch;
		}
	}

	Ceng::Texture2dDesc desc;

	// Configure descriptor

	desc.width = bitmaps[0].width;
	desc.height = bitmaps[0].height;
	desc.format = bitmaps[0].format;
	desc.sRGB = options.sRGB;
	desc.mipLevels = 0;
	desc.arraySize = 0;
	desc.bindFlags = options.bindFlags;
	desc.cpuAccessFlags = options.cpuAccessFlags;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;
	desc.usage = options.usage;

	desc.optionFlags = options.options;

	Ceng::SubResourceData image[6];

	for (Ceng::UINT32 k = 0; k < 6; ++k)
	{
		image[k].sourcePtr = &bitmaps[k].imageBuffer[0];
		image[k].rowPitch = bitmaps[0].pitch;
		image[k].depthPitch = bitmaps[0].imageSize;
	}

	Ceng::CRESULT cresult = renderDevice->CreateCubemap(desc, image, out_texture);

	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	return EngineResult::ok;
}