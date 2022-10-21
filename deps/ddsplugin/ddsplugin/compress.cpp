/*****************************************************************************
*
* compress.cpp
*
* Created By Jari Korkala 16/5/2016
*
*****************************************************************************/

#include <malloc.h>

#include "../include/ddsplugin.h"

extern "C"
{
	DDSPLUGIN_API int Compress_Texture2D_DXT(const char *sourceFile, const char *destFile, int compression,
		int normalMap, int mipmapFilter, int mipmapWrap)
	{
		Bitmap texture;

		int result = Load_PNG(sourceFile, &texture);
		if (result != 1)
		{
			return result;
		}

		INPUT_FORMAT_TYPE inputFormat;

		switch (texture.type)
		{
		case Bitmap::rgb8:
			inputFormat = INPUT_RGB8;
			break;
		case Bitmap::rgba8:
			inputFormat = INPUT_RGBA8;
			break;
		default:
			free(texture.buffer);
			return DDS_UNSUPPORTED_FORMAT;
		}

		DDSWriteVals dds_write_vals;

		dds_write_vals.compression = compression;
		dds_write_vals.format = DDS_FORMAT_DEFAULT;

		if (normalMap == 0)
		{
			dds_write_vals.gamma = 2.2f;
			dds_write_vals.gamma_correct = 1;
			dds_write_vals.srgb = 1;
		}
		else
		{
			dds_write_vals.gamma_correct = 0;
			dds_write_vals.srgb = 0;
		}

		dds_write_vals.mipmaps = DDS_MIPMAP_GENERATE;
		dds_write_vals.perceptual_metric = 0;
		dds_write_vals.mipmap_filter = mipmapFilter;
		dds_write_vals.mipmap_wrap = mipmapWrap;
		dds_write_vals.savetype = DDS_SAVE_SELECTED_LAYER;

		// defaults
		dds_write_vals.transindex = 0;
		dds_write_vals.alpha_test_threshold = 0.5;
		dds_write_vals.preserve_alpha_coverage = 0;

		// write_dds unallocates source buffer, so no need to do it here
		result= write_dds((char*)destFile, &dds_write_vals, texture.buffer, texture.width, texture.height, texture.depth,
			inputFormat, texture.bytesPerPixel, texture.pitch);

		return result;
	}

}