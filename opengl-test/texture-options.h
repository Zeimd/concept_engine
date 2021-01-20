/*****************************************************************************
*
* texture-options.h
*
* By Jari Korkala 30/5/2015
*
*****************************************************************************/

#ifndef CENGINE_TEXTURE_OPTIONS_H
#define CENGINE_TEXTURE_OPTIONS_H

#include <ceng/datatypes.h>

#include <ceng/enums/buffer-usage.h>

namespace CEngine
{
	class TextureOptions
	{
	public:

		Ceng::UINT32 firstMip;

		/**
		* Max  mipmap level. Set to zero to use as many
		* as are available or can be generated.
		*/
		Ceng::UINT32 mipLevels;

		/**
		* Use sRGB format if possible.
		*/
		Ceng::BOOL sRGB;

		/**
		* GPU access mode.
		*/
		Ceng::BufferUsage::value usage;

		/**
		* Combination of Buffer_CPU_Access values.
		*/
		Ceng::UINT32 cpuAccessFlags;

		/**
		* Combination of Ceng::BufferBinding values.
		*/
		Ceng::UINT32 bindFlags;

		/**
		* Combination of Ceng::BufferOptions values.
		*/
		Ceng::UINT32 options;
	};
}

#endif
