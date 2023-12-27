/*****************************************************************************
*
* texture.h
*
* By Jari Korkala 3/11/2015
*
*****************************************************************************/

#ifndef CENGINE_TEXTURE_LINK_H
#define CENGINE_TEXTURE_LINK_H

#include <ceng.h>

namespace CEngine
{
	namespace TextureType
	{
		enum value
		{
			d1 ,
			d2 ,
			d3 ,
			d1_array ,
			d2_array ,
			cubemap ,
		};
	}

	class Texture
	{
	public:
		Ceng::StringUtf8 name;
		TextureType::value type;

		Ceng::StringUtf8 managerLog;

		Ceng::BOOL sRGB;

	public:
		Texture()
		{
		}

		virtual ~Texture()
		{
		}

		virtual Ceng::Texture2D* AsTexture2D()
		{
			return nullptr;
		}

		virtual Ceng::Cubemap* AsCubemap()
		{
			return nullptr;
		}
	};

	class Texture2D : public Texture
	{
	public:
		Ceng::Texture2D *texture;

	public:
		Texture2D()
		{
		}

		virtual ~Texture2D()
		{
			if (texture != nullptr)
			{
				texture->Release();
			}
		}

		virtual Ceng::Texture2D* AsTexture2D() override
		{
			return texture;
		}
	};

	class TextureCube : public Texture
	{
	public:
		Ceng::Cubemap *texture;

		// Corresponding irradiance map if it exists
		TextureCube* relatedIrradiance;

		// Parent cubemap for irradiance map
		TextureCube* irradianceParent;

	public:
		TextureCube()
		{
		}

		virtual ~TextureCube()
		{
			if (texture != nullptr)
			{
				texture->Release();
			}
		}

		virtual Ceng::Cubemap* AsCubemap() override
		{
			return texture;
		}
	};

	class Bitmap
	{
	public:
		Ceng::UINT32 width;
		Ceng::UINT32 height;
		Ceng::IMAGE_FORMAT::value format;
		Ceng::BOOL sRGB;

		Ceng::UINT32 bytesPerPixel;
		Ceng::UINT32 pitch;
		Ceng::UINT32 imageSize;

		std::vector<Ceng::UINT8> imageBuffer;
	};
}

#endif
