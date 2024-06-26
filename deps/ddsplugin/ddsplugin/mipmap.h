/*
	DDS GIMP plugin

	Copyright (C) 2004-2012 Shawn Kirst <skirst@gmail.com>,
   with parts (C) 2003 Arne Reuter <homepage@arnereuter.de> where specified.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; see the file COPYING.  If not, write to
	the Free Software Foundation, 51 Franklin Street, Fifth Floor
	Boston, MA 02110-1301, USA.
*/

#ifndef MIPMAP_H
#define MIPMAP_H

#include "../include/ddsplugin.h"

extern "C"
{
	DDSPLUGIN_API int get_num_mipmaps(int width, int height);
	DDSPLUGIN_API unsigned int get_mipmapped_size(int width, int height, int bpp,
		int level, int num, int format);
	DDSPLUGIN_API unsigned int get_volume_mipmapped_size(int width, int height,
		int depth, int bpp, int level,
		int num, int format);
	DDSPLUGIN_API int get_next_mipmap_dimensions(int *next_w, int *next_h,
		int  curr_w, int  curr_h);

	//DDSPLUGIN_API float cubic_interpolate(float a, float b, float c, float d, float x);

	DDSPLUGIN_API int generate_mipmaps(unsigned char *dst, unsigned char *src,
		unsigned int width, unsigned int height, int bpp,
		int indexed, int mipmaps, int filter, int wrap,
		int gamma_correct, float gamma,
		int preserve_alpha_test_coverage, float alpha_test_threshold);
	DDSPLUGIN_API int generate_volume_mipmaps(unsigned char *dst, unsigned char *src,
		unsigned int width, unsigned int height,
		unsigned int depth, int bpp, int indexed,
		int mipmaps, int filter, int wrap,
		int gamma_correct, float gamma);
}
#endif
