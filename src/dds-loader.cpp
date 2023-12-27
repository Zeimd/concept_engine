/*****************************************************************************
*
* dds-loader.cpp
*
* By Jari Korkala 15/11/2015
*
*****************************************************************************/

#include <fstream>

#include "texture-manager.h"

//#include <dxgi.h>
//#include <d3d10.h>

using namespace CEngine;

typedef enum DXGI_FORMAT {
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,
	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132,
	DXGI_FORMAT_FORCE_UINT = 0xffffffff
} DXGI_FORMAT;

typedef enum D3D10_RESOURCE_DIMENSION {
	D3D10_RESOURCE_DIMENSION_UNKNOWN = 0,
	D3D10_RESOURCE_DIMENSION_BUFFER = 1,
	D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
	D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
	D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
} D3D10_RESOURCE_DIMENSION;

enum DDS_Flag
{
	caps = 1 ,
	height = 2 ,
	width = 4  ,
	pitch = 8 ,
	pixel_format = 0x1000 ,
	mipmap_count = 0x20000 ,
	linear_size = 0x80000 ,
	depth = 0x800000 ,
};

enum DDS_FormatFlag
{
	/**
	 * Texture contains alpha data. rgbAlphaBitmask is valid.
	 */
	alpha_pixels = 1,

	/**
	 * Alpha channel only uncompressed data. rgbBitCount = alpha channel bits,
	 * alphaBitMask = valid data.
	 */
	alpha = 2,

	/**
	* Texture is compressed. fourCC field contains valid data.
	*/
	fourcc = 4,

	/**
	* Texture is uncompressed. rgbBitCount and redBitMask,greenBitMask,blueBitMask
	* contain valid data.
	*/
	rgb = 0x40,

	/**
	 * Texture is in YUV color space. rgbBitCount is YUV bit count. Masks
	 * are red=y , green=u , blue=v.
	 */
	yuv = 0x200,

	/**
	 * Old format. Uncompressed single channel. rgbBitCount = luminance bits.
	 * red mask = luminance mask.
	 * Can be combined with alpha_pixels for a two-channel image.
	 */
	luminance = 0x20000 ,
};

enum DDS_Caps
{
	/**
	 * Optional. Must be used when file contains more than one surface
	 * (mipmap, cubic environment map, mipmapped volume texture).
	 */
	complex = 0x8 ,

	/**
	 * Optional. Should be used for mipmaps.
	 */
	mipmap = 0x400000 ,

	/**
	 * Required.
	 */
	texture = 0x1000 ,
};

enum DDS_Caps2
{
	/**
	 * Required for cube map.
	 */
	cubemap = 0x200 ,
	
	/**
	 * Required when these surfaces are stored in a cube map.
	 */
	cubemap_positive_x = 0x400 ,

	/**
	* Required when these surfaces are stored in a cube map.
	*/
	cubemap_negative_x = 0x800 ,

	/**
	* Required when these surfaces are stored in a cube map.
	*/
	cubemap_positive_y = 0x1000 ,

	/**
	* Required when these surfaces are stored in a cube map.
	*/
	cubemap_negative_y = 0x2000 ,

	/**
	* Required when these surfaces are stored in a cube map.
	*/
	cubemap_positive_z = 0x4000 ,

	/**
	* Required when these surfaces are stored in a cube map.
	*/
	cubemap_negative_z = 0x8000 ,

	/**
	 * Required for a volume texture.
	 */
	volume = 0x200000 ,
};

struct DDS_PixelFormat
{
	/**
	 * Size of DDS_PixelFormat struct in bytes (32).
	 */
	Ceng::INT32 structSize;

	/**
	 * Combination of DDS_FormatFlags values.
	 */
	Ceng::INT32 flags;

	/**
	 * Compression type if used.
	 */
	Ceng::INT32 fourCC;

	/**
	 * Number of bits in RGB data per pixel.
	 */
	Ceng::INT32 rgbBitCount;

	Ceng::INT32 redBitMask;

	Ceng::INT32 greenBitMask;

	Ceng::INT32 blueBitMask;

	Ceng::INT32 alphaBitMask;
};

/**
 * Fields are in the order in which they appear in the file format.
 */
struct DDS_Header
{
	/**
	 * Size of DDS_Header struct. Must be 124.
	 */
	Ceng::INT32 size;
	Ceng::INT32 flags;

	Ceng::INT32 height;
	Ceng::INT32 width;

	Ceng::INT32 pitchOrLinearSize;

	Ceng::INT32 depth;

	Ceng::INT32 mipCount;

	/**
	 * Unused.
	 */
	Ceng::INT32 reserved1[11];

	DDS_PixelFormat format;

	Ceng::INT32 caps;
	Ceng::INT32 caps2;

	/**
	 * Unused.
	 */
	Ceng::INT32 caps3;

	/**
	 * Unused.
	 */
	Ceng::INT32 caps4;

	/**
	 * Unused.
	 */
	Ceng::INT32 reserved2;
};

struct DDS_Header10
{
	DXGI_FORMAT dxgiFormat;

	Ceng::UINT32 miscFlags;
	Ceng::UINT32 arraySize;
	Ceng::UINT32 miscFlags2;
};

Ceng::UINT8 dxt1[4] = { 'D', 'X', 'T', '1' };
Ceng::UINT8 dxt2[4] = { 'D', 'X', 'T', '2' };
Ceng::UINT8 dxt3[4] = { 'D', 'X', 'T', '3' };
Ceng::UINT8 dxt4[4] = { 'D', 'X', 'T', '4' };
Ceng::UINT8 dxt5[4] = { 'D', 'X', 'T', '5' };

Ceng::UINT8 dxh10[4] = { 'D','X','1','0' };

/**
 * FourCC for 3Dc+ normal map compression.
 */
Ceng::UINT8 normal_3dc[4] = { 'A', 'T', 'I', '2' };

const Ceng::UINT32 dds_identifier = 0x20534444;

const EngineResult::value DDS_TranslateFormat(const DDS_PixelFormat &format, Ceng::IMAGE_FORMAT::value *out_format,
	Ceng::BOOL &compressed);

const EngineResult::value DDS_TranslateFormat10(const DDS_PixelFormat &format, 
	const DDS_Header10 &header10,Ceng::IMAGE_FORMAT::value *out_format,Ceng::BOOL &compressed, Ceng::BOOL &sRGB);

const EngineResult::value TextureManager::LoadTexture2D_DDS(const Ceng::StringUtf8 &filename, 
	const CEngine::TextureOptions &options,Ceng::Texture2D **out_texture)
{
	std::ifstream fileReader;

	fileReader.open(filename.ToCString(), std::ios::in | std::ios::binary);
	if (!fileReader.good())
	{
		return EngineResult::file_not_found;
	}

	Ceng::INT8 paramBuffer[256];

	// Test magic number

	fileReader.read(paramBuffer, 4);

	Ceng::INT32 *iptr = (Ceng::INT32*)&paramBuffer[0];
	if (!fileReader.good())
	{
		if (fileReader.eof())
		{
			return EngineResult::unexpected_eof;
		}
		return EngineResult::io_error;
	}

	if (*iptr != dds_identifier)
	{
		return EngineResult::unknown_file_format;
	}

	DDS_Header header;

	fileReader.read((Ceng::INT8*)&header, sizeof(DDS_Header));
	if (!fileReader.good())
	{
		if (fileReader.eof())
		{
			return EngineResult::unexpected_eof;
		}
		return EngineResult::io_error;
	}

	if (header.caps2 & DDS_Caps2::cubemap)
	{
		return EngineResult::invalid_texture_type;
	}

	if (header.caps2 & DDS_Caps2::volume)
	{
		return EngineResult::invalid_texture_type;
	}

	Ceng::Texture2dDesc desc;

	desc.width = header.width;
	desc.height = header.height;
	desc.mipLevels = header.mipCount;

	Ceng::BOOL compressed;

	DDS_Header10 header10;

	Ceng::BOOL sRGB = false;

	if (header.format.flags & DDS_FormatFlag::fourcc)
	{
		Ceng::UINT8 *headerTest = (Ceng::UINT8*)&header.format.fourCC;

		Ceng::UINT32 *match = (Ceng::UINT32*)&dxh10;

		if (*match == header.format.fourCC)
		{
			fileReader.read((Ceng::INT8*)&header10, sizeof(DDS_Header10));
			if (!fileReader.good())
			{
				if (fileReader.eof())
				{
					return EngineResult::unexpected_eof;
				}
				return EngineResult::io_error;
			}


			EngineResult::value eresult = DDS_TranslateFormat10(header.format, header10,&desc.format, compressed,sRGB);
			if (eresult != EngineResult::ok)
			{
				return eresult;
			}
		}
		else
		{
			EngineResult::value eresult = DDS_TranslateFormat(header.format, &desc.format, compressed);
			if (eresult != EngineResult::ok)
			{
				return eresult;
			}
		}
	}



	/*
	if (compressed == true)
	{
		if (options.options & Ceng::BufferOptions::generate_mip_maps)
		{
			return EngineResult::invalid_param;
		}
	}
	*/

	/*
	if (desc.mipLevels != 1)
	{
		if (options.options & Ceng::BufferOptions::generate_mip_maps)
		{
			return EngineResult::invalid_param;
		}
	}
	*/

	if (options.mipLevels != 0)
	{
		/*
		if (options.mipLevels > desc.mipLevels)
		{
			return EngineResult::invalid_param;
		}
		*/
		
		if (options.mipLevels < desc.mipLevels)
		{
			desc.mipLevels = options.mipLevels;
		}
	}

	desc.sRGB = options.sRGB;

	desc.usage = options.usage;
	desc.bindFlags = options.bindFlags;
	desc.optionFlags = 0;
	desc.cpuAccessFlags = options.cpuAccessFlags;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;

	// Calculate data size for base level image

	auto resourceData = std::vector<Ceng::SubResourceData>(desc.mipLevels);

	std::vector<std::vector<Ceng::UINT8>> imageBuffer;

	Ceng::INT32 dataSize = 0;

	if (header.flags & DDS_Flag::linear_size)
	{
		// Compressed texture

		Ceng::INT32 blockSize = 0;

		switch(desc.format)
		{
		case Ceng::IMAGE_FORMAT::dxt1:
			blockSize = 8;
			break;
		case Ceng::IMAGE_FORMAT::dxt2:
		case Ceng::IMAGE_FORMAT::dxt3:
		case Ceng::IMAGE_FORMAT::dxt4:
		case Ceng::IMAGE_FORMAT::dxt5:

		case Ceng::IMAGE_FORMAT::normal_3dc:
		case Ceng::IMAGE_FORMAT::bc5_rg_unorm:
		case Ceng::IMAGE_FORMAT::bc5_rg_snorm:
			blockSize = 16;
			break;
		default:
			return EngineResult::not_supported;
		}

		for (Ceng::UINT32 k = 0; k < desc.mipLevels; ++k)
		{
			// Calculate size of image

			Ceng::INT32 width = desc.width >> k;
			Ceng::INT32 height = desc.height >> k;

			Ceng::INT32 xSize = (width + 3) / 4;
			if (xSize < 1) xSize = 1;

			Ceng::INT32 ySize = (height + 3) / 4;
			if (ySize < 1) ySize = 1;

			Ceng::INT32 size = xSize*ySize*blockSize;

			// Load image data
			imageBuffer.push_back(std::vector<Ceng::UINT8>(size));

			Ceng::INT8 *target = (Ceng::INT8*)&imageBuffer.back()[0];

			fileReader.read(target, size);
			if (!fileReader.good())
			{
				if (fileReader.eof())
				{
					return EngineResult::unexpected_eof;
				}
				return EngineResult::io_error;
			}

			// Write subresource data

			resourceData[k].compressedSize = size;
			resourceData[k].sourcePtr = target;
		}
	}

	Ceng::CRESULT cresult;

	Ceng::Texture2D *texture;

	cresult = renderDevice->CreateTexture2D(desc, &resourceData[0], &texture);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	*out_texture = texture;

	return EngineResult::ok;
}

const EngineResult::value DDS_TranslateFormat10(const DDS_PixelFormat &format,
	const DDS_Header10 &header10, Ceng::IMAGE_FORMAT::value *out_format, Ceng::BOOL &compressed,Ceng::BOOL &sRGB)
{
	compressed = true;
	sRGB = false;

	switch (header10.dxgiFormat)
	{
	case DXGI_FORMAT_BC1_UNORM:
		*out_format = Ceng::IMAGE_FORMAT::dxt1;
		break;
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		*out_format = Ceng::IMAGE_FORMAT::dxt1;
		sRGB = true;
		break;
	case DXGI_FORMAT_BC1_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::dxt1;
		break;
	case DXGI_FORMAT_BC2_UNORM:
		*out_format = Ceng::IMAGE_FORMAT::dxt3;
		break;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		*out_format = Ceng::IMAGE_FORMAT::dxt3;
		sRGB = true;
		break;
	case DXGI_FORMAT_BC2_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::dxt3;
		break;
	case DXGI_FORMAT_BC3_UNORM:
		*out_format = Ceng::IMAGE_FORMAT::dxt5;
		break;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		*out_format = Ceng::IMAGE_FORMAT::dxt5;
		sRGB = true;
		break;
	case DXGI_FORMAT_BC3_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::dxt3;
		break;
	case DXGI_FORMAT_BC4_UNORM:
		*out_format = Ceng::IMAGE_FORMAT::bc4_r_unorm;
		break;
	case DXGI_FORMAT_BC4_SNORM:
		*out_format = Ceng::IMAGE_FORMAT::bc4_r_snorm;
		break;
	case DXGI_FORMAT_BC4_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::bc4_r_unorm;
		break;
	case DXGI_FORMAT_BC5_UNORM:
		*out_format = Ceng::IMAGE_FORMAT::bc5_rg_unorm;
		break;
	case DXGI_FORMAT_BC5_SNORM:
		*out_format = Ceng::IMAGE_FORMAT::bc5_rg_snorm;
		break;
	case DXGI_FORMAT_BC5_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::bc5_rg_unorm;
		break;
	case DXGI_FORMAT_BC6H_UF16:
		*out_format = Ceng::IMAGE_FORMAT::bc6_rgb_unsigned_float;
		break;
	case DXGI_FORMAT_BC6H_SF16:
		*out_format = Ceng::IMAGE_FORMAT::bc6_rgb_float;
		break;
	case DXGI_FORMAT_BC6H_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::bc6_rgb_float;
		break;
	case DXGI_FORMAT_BC7_UNORM:
		*out_format = Ceng::IMAGE_FORMAT::bc7_rgba_unorm;
		break;
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		*out_format = Ceng::IMAGE_FORMAT::bc7_srgb_alpha_unorm;
		break;
	case DXGI_FORMAT_BC7_TYPELESS:
		*out_format = Ceng::IMAGE_FORMAT::bc7_rgba_unorm;
		break;
	default:
		return EngineResult::not_supported;
	}
	return EngineResult::ok;
}

const EngineResult::value DDS_TranslateFormat(const DDS_PixelFormat &format,
	Ceng::IMAGE_FORMAT::value *out_format,Ceng::BOOL &compressed)
{
	compressed = true;
	if (format.flags & DDS_FormatFlag::fourcc)
	{
		// Test for dxt1 

		Ceng::UINT32 *code = (Ceng::UINT32*)&dxt1[0];

		if (format.fourCC == *code)
		{
			*out_format = Ceng::IMAGE_FORMAT::dxt1;
			return EngineResult::ok;
		}

		// Test for dxt2

		code = (Ceng::UINT32*)&dxt2[0];

		if (format.fourCC == *code)
		{
			*out_format = Ceng::IMAGE_FORMAT::dxt2;
			return EngineResult::ok;
		}

		// Test for dxt3

		code = (Ceng::UINT32*)&dxt3[0];

		if (format.fourCC == *code)
		{
			*out_format = Ceng::IMAGE_FORMAT::dxt3;
			return EngineResult::ok;
		}

		// Test for dxt4

		code = (Ceng::UINT32*)&dxt4[0];

		if (format.fourCC == *code)
		{
			*out_format = Ceng::IMAGE_FORMAT::dxt4;
			return EngineResult::ok;
		}

		// Test for dxt5

		code = (Ceng::UINT32*)&dxt5[0];

		if (format.fourCC == *code)
		{
			*out_format = Ceng::IMAGE_FORMAT::dxt5;
			return EngineResult::ok;
		}

		// Test for 3dc

		code = (Ceng::UINT32*)&normal_3dc[0];

		if (format.fourCC == *code)
		{
			//*out_format = Ceng::IMAGE_FORMAT::normal_3dc;
			*out_format = Ceng::IMAGE_FORMAT::bc5_rg_unorm;
			return EngineResult::ok;
		}
	}
	else
	{
		compressed = false;
		if (format.rgbBitCount == 24)
		{

		}
	}

	return EngineResult::not_supported;
}