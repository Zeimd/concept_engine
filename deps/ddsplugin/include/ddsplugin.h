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

#ifndef __DDSPLUGIN_H
#define __DDSPLUGIN_H

#ifdef DDSPLUGIN_EXPORTS
#define DDSPLUGIN_API __declspec(dllexport)
#else
#define DDSPLUGIN_API __declspec(dllimport)
#endif

#include "dds.h"

#define DDS_PLUGIN_VERSION_MAJOR     3
#define DDS_PLUGIN_VERSION_MINOR     0
#define DDS_PLUGIN_VERSION_REVISION  98

#define DDS_PLUGIN_VERSION  \
   ((unsigned int)(DDS_PLUGIN_VERSION_MAJOR << 16) | \
    (unsigned int)(DDS_PLUGIN_VERSION_MINOR <<  8) | \
    (unsigned int)(DDS_PLUGIN_VERSION_REVISION))

typedef struct
{
   int compression;
   int mipmaps;
   int savetype;
   int format;
   int transindex;
   int mipmap_filter;
   int mipmap_wrap;
   int gamma_correct;
   int srgb;
   float gamma;
   int perceptual_metric;
   int show_adv_opt;
   int preserve_alpha_coverage;
   float alpha_test_threshold;
} DDSWriteVals;

typedef struct
{
   int show_dialog;
   int mipmaps;
   int decode_images;
} DDSReadVals;

extern DDSReadVals dds_read_vals;

typedef struct string_value_s
{
	int value;
	char *string;
} string_value_t;

extern "C"
{
	DDSPLUGIN_API int write_dds(char *filename, DDSWriteVals *dds_write_vals,
		void *buffer, int width, int height, int depth,INPUT_FORMAT_TYPE format,
		int bbps, int pitch);
}

//extern GimpPDBStatusType read_dds(gchar *filename, gint32 *imageID);
//extern GimpPDBStatusType write_dds(gchar *, gint32, gint32);

//extern gint interactive_dds;
//extern gchar *prog_name;
//extern gchar *filename;
//extern FILE *errorFile;

#define LOAD_PROC "file-dds-load"
#define SAVE_PROC "file-dds-save"

#define DECODE_YCOCG_PROC "color-decode-ycocg"
#define DECODE_YCOCG_SCALED_PROC "color-decode-ycocg-scaled"
#define DECODE_ALPHA_EXP_PROC "color-decode-alpha-exp"

#include "bitmap.h"

#define DDS_OUTPUT_FILE_CREATION -4
#define DDS_UNSUPPORTED_FORMAT -3
#define DDS_FILE_NOT_FOUND -2
#define DDS_FAILED -1

extern "C"
{
	DDSPLUGIN_API const int Load_PNG(const char *filename, Bitmap *out);

	DDSPLUGIN_API int Compress_Texture2D_DXT(const char *sourceFile, const char *destFile, int compression,
		int normalMap, int mipmapFilter, int mipmapWrap);
}

#endif
