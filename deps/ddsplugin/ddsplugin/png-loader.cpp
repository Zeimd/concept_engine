/*****************************************************************************
*
* png-loader.h
*
* By Jari Korkala 16/6/2016
*
* Uses uPNG library
*
*****************************************************************************/

#include "../include/ddsplugin.h"
#include <stdlib.h>
#include <string.h>

#include <upng.h>

extern "C"
{
	DDSPLUGIN_API const int Load_PNG(const char *filename, Bitmap *out)
	{
		upng_t *png;

		png = upng_new_from_file(filename);
		if (png != nullptr) {
			upng_decode(png);
			int result = upng_get_error(png);
			if (result != UPNG_EOK)
			{
				int error = DDS_FAILED;
				switch (result)
				{
				case UPNG_ENOTFOUND:
					error = DDS_FILE_NOT_FOUND;
					break;
				}
				upng_free(png);
				return error;
			}
		}
		else
		{
			return DDS_FAILED;
		}

		Bitmap bitmap;

		bitmap.width = upng_get_width(png);
		bitmap.height = upng_get_height(png);
		bitmap.depth = 1;
		bitmap.arraySize = 1;

		bitmap.imageSize = upng_get_size(png);

		bitmap.buffer = (unsigned char*)malloc(bitmap.imageSize);

		// Swap blue and red channels

		int format = upng_get_format(png);

		switch (format)
		{
		case UPNG_RGB8:
			bitmap.type = Bitmap::rgb8;
			bitmap.bytesPerPixel = 3;
			break;
		case UPNG_RGBA8:
			bitmap.type = Bitmap::rgba8;
			bitmap.bytesPerPixel = 4;
			break;
		default:
			upng_free(png);
			return DDS_UNSUPPORTED_FORMAT;
		};

		bitmap.pitch = bitmap.width*bitmap.bytesPerPixel;

		memcpy(bitmap.buffer, upng_get_buffer(png), bitmap.imageSize);

		upng_free(png);

		bitmap.sRGB = false;

		*out = bitmap;

		return 1;
	}
}