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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
#include <gtk/gtk.h>
#include <glib/gstdio.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
*/

#include "../include/ddsplugin.h"
#include "dxt.h"
#include "mipmap.h"
#include "endian.h"
#include "imath.h"
#include "color.h"

/*
static gint save_dialog(gint32 image_id, gint32 drawable);
static void save_dialog_response(GtkWidget *widget, gint response_id, gpointer data);
static int write_image(FILE *fp, gint32 image_id, gint32 drawable_id);
*/

static int runme = 0;

enum
{
   COMBO_VALUE, COMBO_STRING, COMBO_SENSITIVE
};

static const char *cubemap_face_names[4][6] =
{
   {
      "positive x", "negative x",
      "positive y", "negative y",
      "positive z", "negative z"
   },
   {
      "pos x", "neg x",
      "pos y", "neg y",
      "pos z", "neg z",
   },
   {
      "+x", "-x",
      "+y", "-y",
      "+z", "-z"
   },
   {
      "right", "left",
      "top", "bottom",
      "back", "front"
   }
};

/*
static gint cubemap_faces[6];
static gint is_cubemap = 0;
static gint is_volume = 0;
static gint is_array = 0;
static gint is_mipmap_chain_valid = 0;

static GtkWidget *compress_opt;
static GtkWidget *format_opt;
static GtkWidget *mipmap_opt;
static GtkWidget *mipmap_filter_opt;
static GtkWidget *mipmap_wrap_opt;
static GtkWidget *srgb_chk;
static GtkWidget *gamma_chk;
static GtkWidget *gamma_spin;
static GtkWidget *pm_chk;
static GtkWidget *alpha_coverage_chk;
static GtkWidget *alpha_test_threshold_spin;
*/


string_value_t compression_strings[] =
{
   {DDS_COMPRESS_NONE,   "None"},
   {DDS_COMPRESS_BC1,    "BC1 / DXT1"},
   {DDS_COMPRESS_BC2,    "BC2 / DXT3"},
   {DDS_COMPRESS_BC3,    "BC3 / DXT5"},
   {DDS_COMPRESS_BC3N,   "BC3nm / DXT5nm"},
   {DDS_COMPRESS_BC4,    "BC4 / ATI1 (3Dc+)"},
   {DDS_COMPRESS_BC5,    "BC5 / ATI2 (3Dc)"},
   {DDS_COMPRESS_RXGB,   "RXGB (DXT5)"},
   {DDS_COMPRESS_AEXP,   "Alpha Exponent (DXT5)"},
   {DDS_COMPRESS_YCOCG,  "YCoCg (DXT5)"},
   {DDS_COMPRESS_YCOCGS, "YCoCg scaled (DXT5)"},
   {-1, 0}
};

static string_value_t format_strings[] =
{
   {DDS_FORMAT_DEFAULT, "Default"},
   {DDS_FORMAT_RGB8,    "RGB8"},
   {DDS_FORMAT_RGBA8,   "RGBA8"},
   {DDS_FORMAT_BGR8,    "BGR8"},
   {DDS_FORMAT_ABGR8,   "ABGR8"},
   {DDS_FORMAT_R5G6B5,  "R5G6B5"},
   {DDS_FORMAT_RGBA4,   "RGBA4"},
   {DDS_FORMAT_RGB5A1,  "RGB5A1"},
   {DDS_FORMAT_RGB10A2, "RGB10A2"},
   {DDS_FORMAT_R3G3B2,  "R3G3B2"},
   {DDS_FORMAT_A8,      "A8"},
   {DDS_FORMAT_L8,      "L8"},
   {DDS_FORMAT_L8A8,    "L8A8"},
   {DDS_FORMAT_AEXP,    "AExp"},
   {DDS_FORMAT_YCOCG,   "YCoCg"},
   {-1, 0}
};

static string_value_t mipmap_strings[] =
{
   {DDS_MIPMAP_NONE,     "No mipmaps"},
   {DDS_MIPMAP_GENERATE, "Generate mipmaps"},
   {DDS_MIPMAP_EXISTING, "Use existing mipmaps"},
   {-1, 0}
};

static string_value_t mipmap_filter_strings[] =
{
   {DDS_MIPMAP_FILTER_DEFAULT,   "Default"},
   {DDS_MIPMAP_FILTER_NEAREST,   "Nearest"},
   {DDS_MIPMAP_FILTER_BOX,       "Box"},
   {DDS_MIPMAP_FILTER_TRIANGLE,  "Triangle"},
   {DDS_MIPMAP_FILTER_QUADRATIC, "Quadratic"},
   {DDS_MIPMAP_FILTER_BSPLINE,   "B-Spline"},
   {DDS_MIPMAP_FILTER_MITCHELL,  "Mitchell"},
   {DDS_MIPMAP_FILTER_LANCZOS,   "Lanczos"},
   {DDS_MIPMAP_FILTER_KAISER,    "Kaiser"},
   {-1, 0}
};

static string_value_t mipmap_wrap_strings[] =
{
   {DDS_MIPMAP_WRAP_DEFAULT, "Default"},
   {DDS_MIPMAP_WRAP_MIRROR,  "Mirror"},
   {DDS_MIPMAP_WRAP_REPEAT,  "Repeat"},
   {DDS_MIPMAP_WRAP_CLAMP,   "Clamp"},
   {-1, 0}
};

static string_value_t save_type_strings[] =
{
   {DDS_SAVE_SELECTED_LAYER, "Image / Selected layer"},
   {DDS_SAVE_CUBEMAP,        "As cube map"},
   {DDS_SAVE_VOLUMEMAP,      "As volume map"},
   {DDS_SAVE_ARRAY,          "As texture array"},
   {-1, 0}
};

static int write_image(FILE *fp, DDSWriteVals *dds_write_vals, void *buffer, int width, int height, int depth, int format, int pitch);

static struct
{
   int format;
   DXGI_FORMAT dxgi_format;
   int bpp;
   int alpha;
   unsigned int rmask;
   unsigned int gmask;
   unsigned int bmask;
   unsigned int amask;
} format_info[] =
{
   {DDS_FORMAT_RGB8,    DXGI_FORMAT_UNKNOWN,           3, 0, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000},
   {DDS_FORMAT_RGBA8,   DXGI_FORMAT_B8G8R8A8_UNORM,    4, 1, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000},
   {DDS_FORMAT_BGR8,    DXGI_FORMAT_UNKNOWN,           3, 0, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000},
   {DDS_FORMAT_ABGR8,   DXGI_FORMAT_R8G8B8A8_UNORM,    4, 1, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000},
   {DDS_FORMAT_R5G6B5,  DXGI_FORMAT_B5G6R5_UNORM,      2, 0, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000},
   {DDS_FORMAT_RGBA4,   DXGI_FORMAT_B4G4R4A4_UNORM,    2, 1, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000},
   {DDS_FORMAT_RGB5A1,  DXGI_FORMAT_B5G5R5A1_UNORM,    2, 1, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000},
   {DDS_FORMAT_RGB10A2, DXGI_FORMAT_R10G10B10A2_UNORM, 4, 1, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000},
   {DDS_FORMAT_R3G3B2,  DXGI_FORMAT_UNKNOWN,           1, 0, 0x000000e0, 0x0000001c, 0x00000003, 0x00000000},
   {DDS_FORMAT_A8,      DXGI_FORMAT_A8_UNORM,          1, 0, 0x00000000, 0x00000000, 0x00000000, 0x000000ff},
   {DDS_FORMAT_L8,      DXGI_FORMAT_R8_UNORM,          1, 0, 0x000000ff, 0x000000ff, 0x000000ff, 0x00000000},
   {DDS_FORMAT_L8A8,    DXGI_FORMAT_UNKNOWN,           2, 1, 0x000000ff, 0x000000ff, 0x000000ff, 0x0000ff00},
   {DDS_FORMAT_AEXP,    DXGI_FORMAT_B8G8R8A8_UNORM,    4, 1, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000},
   {DDS_FORMAT_YCOCG,   DXGI_FORMAT_B8G8R8A8_UNORM,    4, 1, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}
};

extern "C"
{
	DDSPLUGIN_API int write_dds(char *filename, DDSWriteVals *dds_write_vals,void *buffer, 
		int width, int height, int depth,INPUT_FORMAT_TYPE format,int bbps, int pitch)
	{
		FILE *fp;
		int rc = 0;

		fp = fopen(filename, "wb");
		if (fp == 0)
		{
			return DDS_OUTPUT_FILE_CREATION;
			//g_message("Error opening %s", filename);
			//return(GIMP_PDB_EXECUTION_ERROR);
		}


		rc = write_image(fp, dds_write_vals,buffer, width, height, depth,format, pitch);

		fclose(fp);
		return rc;
	}
}
static void swap_rb(unsigned char *pixels, unsigned int n, int bpp)
{
   unsigned int i;
   unsigned char t;

   for(i = 0; i < n; ++i)
   {
      t = pixels[bpp * i + 0];
      pixels[bpp * i + 0] = pixels[bpp * i + 2];
      pixels[bpp * i + 2] = t;
   }
}

static void alpha_exp(unsigned char *dst, int r, int g, int b, int a)
{
   float ar, ag, ab, aa;

   ar = (float)r / 255.0f;
   ag = (float)g / 255.0f;
   ab = (float)b / 255.0f;

   aa = MAX(ar, MAX(ag, ab));

   if(aa < 1e-04f)
   {
      dst[0] = b;
      dst[1] = g;
      dst[2] = r;
      dst[3] = 255;
      return;
   }

   ar /= aa;
   ag /= aa;
   ab /= aa;

   r = (int)floorf(255.0f * ar + 0.5f);
   g = (int)floorf(255.0f * ag + 0.5f);
   b = (int)floorf(255.0f * ab + 0.5f);
   a = (int)floorf(255.0f * aa + 0.5f);

   dst[0] = MAX(0, MIN(255, b));
   dst[1] = MAX(0, MIN(255, g));
   dst[2] = MAX(0, MIN(255, r));
   dst[3] = MAX(0, MIN(255, a));
}

static void convert_pixels(unsigned char *dst, unsigned char *src,
                           int format, int w, int h, int d, int bpp,
                           unsigned char *palette, int mipmaps)
{
   unsigned int i, num_pixels;
   unsigned char r, g, b, a;

   if(d > 0)
      num_pixels = get_volume_mipmapped_size(w, h, d, 1, 0, mipmaps, DDS_COMPRESS_NONE);
   else
      num_pixels = get_mipmapped_size(w, h, 1, 0, mipmaps, DDS_COMPRESS_NONE);

   for(i = 0; i < num_pixels; ++i)
   {
      if(bpp == 1)
      {
         if(palette)
         {
            r = palette[3 * src[i] + 0];
            g = palette[3 * src[i] + 1];
            b = palette[3 * src[i] + 2];
         }
         else
            r = g = b = src[i];

         if(format == DDS_FORMAT_A8)
            a = src[i];
         else
            a = 255;
      }
      else if(bpp == 2)
      {
         r = g = b = src[2 * i];
         a = src[2 * i + 1];
      }
      else if(bpp == 3)
      {
         b = src[3 * i + 0];
         g = src[3 * i + 1];
         r = src[3 * i + 2];
         a = 255;
      }
      else
      {
         b = src[4 * i + 0];
         g = src[4 * i + 1];
         r = src[4 * i + 2];
         a = src[4 * i + 3];
      }

      switch(format)
      {
         case DDS_FORMAT_RGB8:
            dst[3 * i + 0] = b;
            dst[3 * i + 1] = g;
            dst[3 * i + 2] = r;
            break;
         case DDS_FORMAT_RGBA8:
            dst[4 * i + 0] = b;
            dst[4 * i + 1] = g;
            dst[4 * i + 2] = r;
            dst[4 * i + 3] = a;
            break;
         case DDS_FORMAT_BGR8:
            dst[3 * i + 0] = r;
            dst[3 * i + 1] = g;
            dst[3 * i + 2] = b;
            break;
         case DDS_FORMAT_ABGR8:
            dst[4 * i + 0] = r;
            dst[4 * i + 1] = g;
            dst[4 * i + 2] = b;
            dst[4 * i + 3] = a;
            break;
         case DDS_FORMAT_R5G6B5:
            PUTL16(&dst[2 * i], pack_r5g6b5(r, g, b));
            break;
         case DDS_FORMAT_RGBA4:
            PUTL16(&dst[2 * i], pack_rgba4(r, g, b, a));
            break;
         case DDS_FORMAT_RGB5A1:
            PUTL16(&dst[2 * i], pack_rgb5a1(r, g, b, a));
            break;
         case DDS_FORMAT_RGB10A2:
            PUTL32(&dst[4 * i], pack_rgb10a2(r, g, b, a));
            break;
         case DDS_FORMAT_R3G3B2:
            dst[i] = pack_r3g3b2(r, g, b);
            break;
         case DDS_FORMAT_A8:
            dst[i] = a;
            break;
         case DDS_FORMAT_L8:
            dst[i] = rgb_to_luminance(r, g, b);
            break;
         case DDS_FORMAT_L8A8:
            dst[2 * i + 0] = rgb_to_luminance(r, g, b);
            dst[2 * i + 1] = a;
            break;
         case DDS_FORMAT_YCOCG:
            dst[4 * i] = a;
            RGB_to_YCoCg(&dst[4 * i], r, g, b);
            break;
         case DDS_FORMAT_AEXP:
            alpha_exp(&dst[4 * i], r, g, b, a);
            break;
         default:
            break;
      }
   }
}

/*
static void get_mipmap_chain(unsigned char *dst, int w, int h, int bpp,
                             unsigned char *src)
{

   gint *layers, num_layers;
   GimpDrawable *drawable;
   GimpPixelRgn rgn;

   int i, idx = 0, offset, mipw, miph;
   
   layers = gimp_image_get_layers(image_id, &num_layers);
   
   for(i = 0; i < num_layers; ++i)
   {
      if(layers[i] == drawable_id)
      {
         idx = i;
         break;
      }
   }
   
   if(i == num_layers) return;
  
   offset = 0;

   while(get_next_mipmap_dimensions(&mipw, &miph, w, h))
   {
      //drawable = gimp_drawable_get(layers[++idx]);

      if((width != mipw) || (height != miph)) return;
      
      gimp_pixel_rgn_init(&rgn, drawable, 0, 0, mipw, miph, 0, 0);
      gimp_pixel_rgn_get_rect(&rgn, dst + offset, 0, 0, mipw, miph);

      // we need BGRX or BGRA 
      if(bpp >= 3)
         swap_rb(dst + offset, mipw * miph, bpp);

      offset += (mipw * miph * bpp);
      w = mipw;
      h = miph;

   }
}
*/

static void write_layer(FILE *fp, DDSWriteVals *dds_write_vals,void *buffer, int w, int h, int bpp, int fmtbpp, int mipmaps)
{
   unsigned char *src, *dst, *fmtdst, *tmp;
   unsigned char *palette = NULL;
   int i, c, x, y, size, fmtsize, offset, colors;
   int compression = dds_write_vals->compression;
   int flags = 0;

   src = (unsigned char*)buffer;
 
   /*
   if(basetype == GIMP_INDEXED)
   {
      palette = gimp_image_get_colormap(image_id, &colors);

      if(type == GIMP_INDEXEDA_IMAGE)
      {
         tmp = g_malloc(w * h);
         for(i = 0; i < w * h; ++i)
            tmp[i] = src[2 * i];
         g_free(src);
         src = tmp;
         bpp = 1;
      }
   }
   */

   // we want and assume BGRA ordered pixels for bpp >= 3 from here and
   //   onwards
   if(bpp >= 3)
      swap_rb(src, w * h, bpp);

   if(compression == DDS_COMPRESS_BC3N)
   {
      if(bpp != 4)
      {
         fmtsize = w * h * 4;
         fmtdst = (unsigned char*)malloc(fmtsize);
         convert_pixels(fmtdst, src, DDS_FORMAT_RGBA8, w, h, 0, bpp,
                        palette, 1);
         free(src);
         src = fmtdst;
         bpp = 4;
      }

      for(y = 0; y < h; ++y)
      {
         for(x = 0; x < w; ++x)
         {
			// Normalize vectors
			 float normalX = float(src[y * (w * 4) + (x * 4) + 2]) / 255.0f;
			 float normalY = float(src[y * (w * 4) + (x * 4) + 1]) / 255.0f;
			 float normalZ = float(src[y * (w * 4) + (x * 4)]) / 255.0f;

			 normalX = 2.0f*normalX - 1.0f;
			 normalY = 2.0f*normalY - 1.0f;

			 float div = 1.0f / sqrt(normalX*normalX + normalY*normalY + normalZ*normalZ);

			 normalX *= div;
			 normalY *= div;
			 //normalZ *= div;

			 normalX = 0.5f*normalX + 0.5f;
			 normalY = 0.5f*normalY + 0.5f;

			 unsigned char a = 255.0f*normalX;
			 unsigned char g = 255.0f*normalY;

			 
            // set alpha to red (x) 
            src[y * (w * 4) + (x * 4) + 3] = a;

			src[y * (w * 4) + (x * 4) + 1] = g;

            // set red to 1 
            src[y * (w * 4) + (x * 4) + 2] = 255;
			// set blue to 1 
			src[y * (w * 4) + (x * 4)] = 255;
         }
      }
   }

   // RXGB (Doom3) 
   if(compression == DDS_COMPRESS_RXGB)
   {
      if(bpp != 4)
      {
         fmtsize = w * h * 4;
         fmtdst = (unsigned char*)malloc(fmtsize);
         convert_pixels(fmtdst, src, DDS_FORMAT_RGBA8, w, h, 0, bpp,
                        palette, 1);
         free(src);
         src = fmtdst;
         bpp = 4;
      }

      for(y = 0; y < h; ++y)
      {
         for(x = 0; x < w; ++x)
         {
            // swap red and alpha 
            c = src[y * (w * 4) + (x * 4) + 3];
            src[y * (w * 4) + (x * 4) + 3] =
               src[y * (w * 4) + (x * 4) + 2];
            src[y * (w * 4) + (x * 4) + 2] = c;
         }
      }
   }

   /*
   if(compression == DDS_COMPRESS_YCOCG ||
      compression == DDS_COMPRESS_YCOCGS) // convert to YCoCG
   {
      fmtsize = w * h * 4;
      fmtdst = g_malloc(fmtsize);
      convert_pixels(fmtdst, src, DDS_FORMAT_YCOCG, w, h, 0, bpp,
                     palette, 1);
      g_free(src);
      src = fmtdst;
      bpp = 4;
   }
   */

   /*
   if(compression == DDS_COMPRESS_AEXP)
   {
      fmtsize = w * h * 4;
      fmtdst = g_malloc(fmtsize);
      convert_pixels(fmtdst, src, DDS_FORMAT_AEXP, w, h, 0, bpp,
                     palette, 1);
      g_free(src);
      src = fmtdst;
      bpp = 4;
   }
   */

   if(compression == DDS_COMPRESS_NONE)
   {
	   /*
      if(mipmaps > 1)
      {
         // pre-convert indexed images to RGB for better quality mipmaps
         //   if a pixel format conversion is requested 
         if(dds_write_vals.format > DDS_FORMAT_DEFAULT && basetype == GIMP_INDEXED)
         {
            fmtsize = get_mipmapped_size(w, h, 3, 0, mipmaps, DDS_COMPRESS_NONE);
            fmtdst = g_malloc(fmtsize);
            convert_pixels(fmtdst, src, DDS_FORMAT_RGB8, w, h, 0, bpp,
                           palette, 1);
            g_free(src);
            src = fmtdst;
            bpp = 3;
            palette = NULL;
         }

         size = get_mipmapped_size(w, h, bpp, 0, mipmaps, DDS_COMPRESS_NONE);
         dst = g_malloc(size);
         if(dds_write_vals.mipmaps == DDS_MIPMAP_GENERATE)
         {
            generate_mipmaps(dst, src, w, h, bpp, palette != NULL,
                             mipmaps,
                             dds_write_vals.mipmap_filter,
                             dds_write_vals.mipmap_wrap,
                             dds_write_vals.gamma_correct + dds_write_vals.srgb,
                             dds_write_vals.gamma,
                             dds_write_vals.preserve_alpha_coverage,
                             dds_write_vals.alpha_test_threshold);
         }
         else
         {
            memcpy(dst, src, w * h * bpp);
            get_mipmap_chain(dst + (w * h * bpp), w, h, bpp, image_id, drawable_id);
         }

         if(dds_write_vals.format > DDS_FORMAT_DEFAULT)
         {
            fmtsize = get_mipmapped_size(w, h, fmtbpp, 0, mipmaps,
                                         DDS_COMPRESS_NONE);
            fmtdst = g_malloc(fmtsize);

            convert_pixels(fmtdst, dst, dds_write_vals.format, w, h, 0, bpp,
                           palette, mipmaps);

            g_free(dst);
            dst = fmtdst;
            bpp = fmtbpp;
         }

         offset = 0;

         for(i = 0; i < mipmaps; ++i)
         {
            size = get_mipmapped_size(w, h, bpp, i, 1, DDS_COMPRESS_NONE);
            fwrite(dst + offset, 1, size, fp);
            offset += size;
         }

         g_free(dst);
      }
      else
      {
         if(dds_write_vals.format > DDS_FORMAT_DEFAULT)
         {
            fmtdst = g_malloc(h * w * fmtbpp);
            convert_pixels(fmtdst, src, dds_write_vals.format, w, h, 0, bpp,
                           palette, 1);
            g_free(src);
            src = fmtdst;
            bpp = fmtbpp;
         }

         fwrite(src, 1, h * w * bpp, fp);
      }
	  */
   }
   else
   {
      size = get_mipmapped_size(w, h, bpp, 0, mipmaps, compression);

      dst = (unsigned char*)malloc(size);

	  /*
      if(basetype == GIMP_INDEXED)
      {
         fmtsize = get_mipmapped_size(w, h, 3, 0, mipmaps,
                                      DDS_COMPRESS_NONE);
         fmtdst = g_malloc(fmtsize);
         convert_pixels(fmtdst, src, DDS_FORMAT_RGB8, w, h, 0, bpp,
                        palette, mipmaps);
         g_free(src);
         src = fmtdst;
         bpp = 3;
      }
	  */

      if(mipmaps > 1)
      {
         fmtsize = get_mipmapped_size(w, h, bpp, 0, mipmaps,
                                      DDS_COMPRESS_NONE);
         fmtdst = (unsigned char*)malloc(fmtsize);
         if(dds_write_vals->mipmaps == DDS_MIPMAP_GENERATE)
         {
            generate_mipmaps(fmtdst, src, w, h, bpp, 0, mipmaps,
                             dds_write_vals->mipmap_filter,
                             dds_write_vals->mipmap_wrap,
                             dds_write_vals->gamma_correct + dds_write_vals->srgb,
                             dds_write_vals->gamma,
                             dds_write_vals->preserve_alpha_coverage,
                             dds_write_vals->alpha_test_threshold);
         }
         else
         {
            //memcpy(fmtdst, src, w * h * bpp);
            //get_mipmap_chain(fmtdst + (w * h * bpp), w, h, bpp, buffer);
         }

         free(src);
         src = fmtdst;
      }

      flags = 0;
      if(dds_write_vals->perceptual_metric) flags |= DXT_PERCEPTUAL;

      dxt_compress(dst, src, compression, w, h, bpp, mipmaps, flags);

      fwrite(dst, 1, size, fp);

      free(dst);
   }

   free(src);
}

/*
static void write_volume_mipmaps(FILE *fp, gint32 image_id, gint32 *layers,
                                 int w, int h, int d, int bpp, int fmtbpp,
                                 int mipmaps)
{
   int i, size, offset, colors;
   unsigned char *src, *dst, *tmp, *fmtdst;
   unsigned char *palette = 0;
   GimpDrawable *drawable;
   GimpPixelRgn rgn;
   GimpImageBaseType type;

   type = gimp_image_base_type(image_id);

   if(dds_write_vals.compression != DDS_COMPRESS_NONE) return;

   src = g_malloc(w * h * bpp * d);

   if(gimp_image_base_type(image_id) == GIMP_INDEXED)
      palette = gimp_image_get_colormap(image_id, &colors);

   offset = 0;
   for(i = 0; i < d; ++i)
   {
      drawable = gimp_drawable_get(layers[i]);
      gimp_pixel_rgn_init(&rgn, drawable, 0, 0, w, h, 0, 0);
      gimp_pixel_rgn_get_rect(&rgn, src + offset, 0, 0, w, h);
      offset += (w * h * bpp);
      gimp_drawable_detach(drawable);
   }

   if(gimp_drawable_type(layers[0]) == GIMP_INDEXEDA_IMAGE)
   {
      tmp = g_malloc(w * h * d);
      for(i = 0; i < w * h * d; ++i)
         tmp[i] = src[2 * i];
      g_free(src);
      src = tmp;
      bpp = 1;
   }

   // we want and assume BGRA ordered pixels for bpp >= 3 from here and
   //   onwards 
   if(bpp >= 3)
      swap_rb(src, w * h * d, bpp);

   // pre-convert indexed images to RGB for better mipmaps if a
   //   pixel format conversion is requested 
   if(dds_write_vals.format > DDS_FORMAT_DEFAULT && type == GIMP_INDEXED)
   {
      size = get_volume_mipmapped_size(w, h, d, 3, 0, mipmaps,
                                       DDS_COMPRESS_NONE);
      dst = g_malloc(size);
      convert_pixels(dst, src, DDS_FORMAT_RGB8, w, h, d, bpp, palette, 1);
      g_free(src);
      src = dst;
      bpp = 3;
      palette = NULL;
   }

   size = get_volume_mipmapped_size(w, h, d, bpp, 0, mipmaps,
                                    dds_write_vals.compression);

   dst = g_malloc(size);

   offset = get_volume_mipmapped_size(w, h, d, bpp, 0, 1,
                                      dds_write_vals.compression);

   generate_volume_mipmaps(dst, src, w, h, d, bpp,
                           palette != NULL, mipmaps,
                           dds_write_vals.mipmap_filter,
                           dds_write_vals.mipmap_wrap,
                           dds_write_vals.gamma_correct + dds_write_vals.srgb,
                           dds_write_vals.gamma);

   if(dds_write_vals.format > DDS_FORMAT_DEFAULT)
   {
      size = get_volume_mipmapped_size(w, h, d, fmtbpp, 0, mipmaps,
                                       dds_write_vals.compression);
      offset = get_volume_mipmapped_size(w, h, d, fmtbpp, 0, 1,
                                         dds_write_vals.compression);
      fmtdst = g_malloc(size);

      convert_pixels(fmtdst, dst, dds_write_vals.format, w, h, d, bpp,
                     palette, mipmaps);
      g_free(dst);
      dst = fmtdst;
   }

   fwrite(dst + offset, 1, size, fp);

   g_free(src);
   g_free(dst);
}
*/

static int write_image(FILE *fp, DDSWriteVals *dds_write_vals,void *buffer,int width,int height,int depth,int format,int pitch)
{
   int i, w, h, bpp = 0, fmtbpp = 0, has_alpha = 0;
   int num_mipmaps;
   unsigned char hdr[DDS_HEADERSIZE], hdr10[DDS_HEADERSIZE_DX10];
   unsigned int flags = 0, pflags = 0, caps = 0, caps2 = 0, size = 0;
   unsigned int rmask = 0, gmask = 0, bmask = 0, amask = 0;
   unsigned int fourcc = 0;
   DXGI_FORMAT dxgi_format = DXGI_FORMAT_UNKNOWN;
  
   unsigned char zero[4] = {0, 0, 0, 0};
   int is_dx10 = 0, array_size = 1;
  
   w = width;
   h = height;
  
   switch(format)
   {
      case INPUT_RGB8:      
		  bpp = 3; 
		  break;
      case INPUT_RGBA8:  
		  bpp = 4; 
		  break;
      case INPUT_GRAY8:    
		  bpp = 1; 
		  break;
      case INPUT_GRAY8_ALPHA8:    
		  bpp = 2; 
		  break;
      default:
         break;
   }

   if(dds_write_vals->format > DDS_FORMAT_DEFAULT)
   {
      for(i = 0; ; ++i)
      {
         if(format_info[i].format == dds_write_vals->format)
         {
            fmtbpp = format_info[i].bpp;
            has_alpha = format_info[i].alpha;
            rmask = format_info[i].rmask;
            gmask = format_info[i].gmask;
            bmask = format_info[i].bmask;
            amask = format_info[i].amask;
            dxgi_format = format_info[i].dxgi_format;
            break;
         }
      }
   }
   else if(bpp == 1)
   {
         fmtbpp = 1;
         has_alpha = 0;
         rmask = 0x000000ff;
         gmask = bmask = amask = 0;
         dxgi_format = DXGI_FORMAT_R8_UNORM;
   }
   else if(bpp == 2)
   {
        fmtbpp = 2;
        has_alpha = 1;
        rmask = 0x000000ff;
        gmask = 0x000000ff;
        bmask = 0x000000ff;
        amask = 0x0000ff00;   
   }
   else if(bpp == 3)
   {
      fmtbpp = 3;
      rmask = 0x00ff0000;
      gmask = 0x0000ff00;
      bmask = 0x000000ff;
      amask = 0x00000000;
   }
   else
   {
      fmtbpp = 4;
      has_alpha = 1;
      rmask = 0x00ff0000;
      gmask = 0x0000ff00;
      bmask = 0x000000ff;
      amask = 0xff000000;
      dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
   }

   memset(hdr, 0, DDS_HEADERSIZE);

   PUTL32(hdr,       FOURCC('D','D','S',' '));
   PUTL32(hdr + 4,   124);
   PUTL32(hdr + 12,  h);
   PUTL32(hdr + 16,  w);
   PUTL32(hdr + 76,  32);

   if(dds_write_vals->compression == DDS_COMPRESS_NONE)
   {
      PUTL32(hdr + 88,  fmtbpp << 3);
      PUTL32(hdr + 92,  rmask);
      PUTL32(hdr + 96,  gmask);
      PUTL32(hdr + 100, bmask);
      PUTL32(hdr + 104, amask);
   }

   /*
    put some information in the reserved area to identify the origin
    of the image1
   */
   PUTL32(hdr + 32, FOURCC('G','I','M','P'));
   PUTL32(hdr + 36, FOURCC('-','D','D','S'));
   PUTL32(hdr + 40, DDS_PLUGIN_VERSION);

   flags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT;

   caps = DDSCAPS_TEXTURE;
   if(dds_write_vals->mipmaps)
   {
      flags |= DDSD_MIPMAPCOUNT;
      caps |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
      num_mipmaps = get_num_mipmaps(w, h);
   }
   else
      num_mipmaps = 1;

   if((dds_write_vals->savetype == DDS_SAVE_CUBEMAP))
   {
      caps |= DDSCAPS_COMPLEX;
      caps2 |= (DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_ALL_FACES);
   }
   else if((dds_write_vals->savetype == DDS_SAVE_VOLUMEMAP))
   {
      PUTL32(hdr + 24, depth); /* depth */
      flags |= DDSD_DEPTH;
      caps |= DDSCAPS_COMPLEX;
      caps2 |= DDSCAPS2_VOLUME;
   }

   PUTL32(hdr + 28,  num_mipmaps);
   PUTL32(hdr + 108, caps);
   PUTL32(hdr + 112, caps2);

   if(dds_write_vals->compression == DDS_COMPRESS_NONE)
   {
      flags |= DDSD_PITCH;

      if(dds_write_vals->format > DDS_FORMAT_DEFAULT)
      {
         if(dds_write_vals->format == DDS_FORMAT_A8)
            pflags |= DDPF_ALPHA;
         else
         {
            if(((fmtbpp == 1) || (dds_write_vals->format == DDS_FORMAT_L8A8)) &&
               (dds_write_vals->format != DDS_FORMAT_R3G3B2))
               pflags |= DDPF_LUMINANCE;
            else
               pflags |= DDPF_RGB;
         }
      }
      else
      {
         if(bpp == 1)
         {
          pflags |= DDPF_LUMINANCE;
         }
         else
            pflags |= DDPF_RGB;
      }

      if(has_alpha) pflags |= DDPF_ALPHAPIXELS;

      PUTL32(hdr + 8,  flags);
      PUTL32(hdr + 20, w * fmtbpp); /* pitch */
      PUTL32(hdr + 80, pflags);

      /*
       write extra fourcc info - this is special to GIMP DDS. When the image
       is read by the plugin, we can detect the added information to decode
       the pixels
      */
      if(dds_write_vals->format == DDS_FORMAT_AEXP)
      {
         PUTL32(hdr + 44, FOURCC('A','E','X','P'));
      }
      else if(dds_write_vals->format == DDS_FORMAT_YCOCG)
      {
         PUTL32(hdr + 44, FOURCC('Y','C','G','1'));
      }
   }
   else
   {
      flags |= DDSD_LINEARSIZE;
      pflags = DDPF_FOURCC;

      switch(dds_write_vals->compression)
      {
         case DDS_COMPRESS_BC1:
            fourcc = FOURCC('D','X','T','1');
            dxgi_format = DXGI_FORMAT_BC1_UNORM;
            break;
         case DDS_COMPRESS_BC2:
            fourcc = FOURCC('D','X','T','3');
            dxgi_format = DXGI_FORMAT_BC2_UNORM;
            break;
         case DDS_COMPRESS_BC3:
         case DDS_COMPRESS_BC3N:
         case DDS_COMPRESS_YCOCG:
         case DDS_COMPRESS_YCOCGS:
         case DDS_COMPRESS_AEXP:
            fourcc = FOURCC('D','X','T','5');
            dxgi_format = DXGI_FORMAT_BC3_UNORM;
            break;
         case DDS_COMPRESS_RXGB:
            fourcc = FOURCC('R','X','G','B');
            dxgi_format = DXGI_FORMAT_BC3_UNORM;
            break;
         case DDS_COMPRESS_BC4:
            fourcc = FOURCC('A','T','I','1');
            dxgi_format = DXGI_FORMAT_BC4_UNORM;
            //is_dx10 = 1;
            break;
         case DDS_COMPRESS_BC5:
            fourcc = FOURCC('A','T','I','2');
            dxgi_format = DXGI_FORMAT_BC5_UNORM;
            //is_dx10 = 1;
            break;
      }

      if((dds_write_vals->compression == DDS_COMPRESS_BC3N) ||
         (dds_write_vals->compression == DDS_COMPRESS_RXGB))
         pflags |= DDPF_NORMAL;

      PUTL32(hdr + 8,  flags);
      PUTL32(hdr + 80, pflags);
      PUTL32(hdr + 84, fourcc);

      size = ((w + 3) >> 2) * ((h + 3) >> 2);
      if((dds_write_vals->compression == DDS_COMPRESS_BC1) ||
         (dds_write_vals->compression == DDS_COMPRESS_BC4))
         size *= 8;
      else
         size *= 16;

      PUTL32(hdr + 20, size); /* linear size */

      /*
       write extra fourcc info - this is special to GIMP DDS. When the image
       is read by the plugin, we can detect the added information to decode
       the pixels
      */
      if(dds_write_vals->compression == DDS_COMPRESS_AEXP)
      {
         PUTL32(hdr + 44, FOURCC('A','E','X','P'));
      }
      else if(dds_write_vals->compression == DDS_COMPRESS_YCOCG)
      {
         PUTL32(hdr + 44, FOURCC('Y','C','G','1'));
      }
      else if(dds_write_vals->compression == DDS_COMPRESS_YCOCGS)
      {
         PUTL32(hdr + 44, FOURCC('Y','C','G','2'));
      }
   }

   /* texture arrays require a DX10 header */
   if(dds_write_vals->savetype == DDS_SAVE_ARRAY)
      is_dx10 = 1;

   if(is_dx10)
   {
	  /*
      array_size = (dds_write_vals.savetype == DDS_SAVE_SELECTED_LAYER) ? 1 : get_array_size(image_id);

      PUTL32(hdr10 +  0, dxgi_format);
      PUTL32(hdr10 +  4, D3D10_RESOURCE_DIMENSION_TEXTURE2D);
      PUTL32(hdr10 +  8, 0);
      PUTL32(hdr10 + 12, array_size);
      PUTL32(hdr10 + 16, 0);

      // update main header accordingly
      PUTL32(hdr + 80, pflags | DDPF_FOURCC);
      PUTL32(hdr + 84, FOURCC('D','X','1','0'));
	  */
   }

   fwrite(hdr, DDS_HEADERSIZE, 1, fp);

   if(is_dx10)
      fwrite(hdr10, DDS_HEADERSIZE_DX10, 1, fp);

   // write palette for indexed images
   /*
   if((basetype == GIMP_INDEXED) &&
      (dds_write_vals.format == DDS_FORMAT_DEFAULT) &&
      (dds_write_vals.compression == DDS_COMPRESS_NONE))
   {
      cmap = gimp_image_get_colormap(image_id, &colors);
      for(i = 0; i < colors; ++i)
      {
         fwrite(&cmap[3 * i], 1, 3, fp);
         if(i == dds_write_vals.transindex)
            fputc(0, fp);
         else
            fputc(255, fp);
      }
      for(; i < 256; ++i)
         fwrite(zero, 1, 4, fp);
   }
   */

   if(dds_write_vals->savetype == DDS_SAVE_CUBEMAP)
   {
	   /*
      for(i = 0; i < 6; ++i)
      {
         write_layer(fp, image_id, cubemap_faces[i], w, h, bpp, fmtbpp,
                     num_mipmaps);
         gimp_progress_update((float)(i + 1) / 6.0);
      }
	  */
   }
   else if(dds_write_vals->savetype == DDS_SAVE_VOLUMEMAP)
   {
	   /*
      for(i = 0; i < num_layers; ++i)
      {
         write_layer(fp, image_id, layers[i], w, h, bpp, fmtbpp, 1);
         gimp_progress_update((float)i / (float)num_layers);
      }

      if(num_mipmaps > 1)
         write_volume_mipmaps(fp, image_id, layers, w, h, num_layers,
                              bpp, fmtbpp, num_mipmaps);
							  */
   }
   else if(dds_write_vals->savetype == DDS_SAVE_ARRAY)
   {
	   /*
      for(i = 0; i < num_layers; ++i)
      {
         if((gimp_drawable_width(layers[i]) == w) && (gimp_drawable_height(layers[i]) == h))
            write_layer(fp, image_id, layers[i], w, h, bpp, fmtbpp, num_mipmaps);
         
         gimp_progress_update((float)i / (float)num_layers);
      }
	  */
   }
   else
   {
      write_layer(fp,dds_write_vals,buffer, w, h, bpp, fmtbpp, num_mipmaps);
   }

   return(1);
}

