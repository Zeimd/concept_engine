/*****************************************************************************
*
* bitmap.h
*
* Created By Jari Korkala 16/5/2016
*
*****************************************************************************/

#ifndef BITMAP_H
#define BITMAP_H

struct Bitmap
{
	enum type
	{
		rgb8 = 1,
		rgba8 = 2,
	};

	type type;

	int width;
	int height;
	int depth;
	int arraySize;

	bool sRGB;

	int bytesPerPixel;
	int pitch;
	int imageSize;

	unsigned char *buffer;
}; 

#endif