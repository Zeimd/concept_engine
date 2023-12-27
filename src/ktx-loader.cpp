/*****************************************************************************
*
* ktx-loader.cpp
*
* Created By Jari Korkala 26/10/2015
*
*****************************************************************************/

#include <ceng.h>
#include <iostream>

#include <gl/glew.h>

#include "texture-manager.h"
#include "byte-order.h"

using namespace CEngine;

const Ceng::UINT8 ktx_identifier[] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

const EngineResult::value TextureManager::LoadTexture2D_KTX(const Ceng::StringUtf8 &filename,
	const CEngine::TextureOptions &options,Ceng::Texture2D **out_texture)
{
	if (renderDevice == nullptr)
	{
		return EngineResult::invalid_param;
	}

	std::ifstream fileReader;

	fileReader.open(filename.ToCString(), std::ios::in | std::ios::binary);
	if (!fileReader.good())
	{
		return EngineResult::file_not_found;
	}

	Ceng::INT8 paramBuffer[32 * 4];

	// Test format identifier

	fileReader.read(paramBuffer, 12);
	if (!fileReader.good())
	{
		return EngineResult::io_error;
	}

	for (Ceng::UINT32 k = 0; k < 12; ++k)
	{
		if (ktx_identifier[k] != (Ceng::UINT8)paramBuffer[k])
		{
			return EngineResult::unknown_file_format;
		}
	}

	// Check endianess

	fileReader.read(paramBuffer, 4);
	if (!fileReader.good())
	{
		return EngineResult::io_error;
	}

	Ceng::INT32 *intValue = (Ceng::INT32*)&paramBuffer[0];

	Ceng::BOOL swapByteOrder = false;

	if (*intValue == 0x01020304)
	{
		swapByteOrder = true;
	}

	// Read texture information
	fileReader.read(paramBuffer, 12 * 4);
	if (!fileReader.good())
	{
		return EngineResult::io_error;
	}

	GLint gl_datatype = CorrectByteOrder(intValue[0], swapByteOrder);
	GLint gl_typeSize = CorrectByteOrder(intValue[1], swapByteOrder);
	GLint gl_format = CorrectByteOrder(intValue[2], swapByteOrder);
	GLint gl_internalFormat = CorrectByteOrder(intValue[3], swapByteOrder);
	GLint gl_baseInternalFormat = CorrectByteOrder(intValue[4], swapByteOrder);

	GLint width = CorrectByteOrder(intValue[5], swapByteOrder);
	GLint height = CorrectByteOrder(intValue[6], swapByteOrder);
	GLint depth = CorrectByteOrder(intValue[7], swapByteOrder);

	GLint arrayElements = CorrectByteOrder(intValue[8], swapByteOrder);
	GLint faces = CorrectByteOrder(intValue[9], swapByteOrder);
	GLint mipLevels = CorrectByteOrder(intValue[10], swapByteOrder);

	Ceng::INT32 bytesOfKeyValueData = CorrectByteOrder(intValue[11], swapByteOrder);

	// Skip keyvalue data

	fileReader.seekg(bytesOfKeyValueData, std::ios::cur);
	if (!fileReader.good())
	{
		return EngineResult::io_error;
	}

	Ceng::INT32 bytesPerPixel = gl_typeSize;

	Ceng::INT32 maxMipLevels = mipLevels;
	if (maxMipLevels == 0)
	{
		maxMipLevels = 1;
	}

	Ceng::INT32 maxArrayIndex = arrayElements;
	if (maxArrayIndex == 0)
	{
		maxArrayIndex = 1;
	}

	Ceng::INT32 maxDepth = depth;
	if (maxDepth == 0)
	{
		maxDepth = 1;
	}

	GLint textureType = 0;

	if (faces == 6)
	{
		if (depth > 0)
		{
			return EngineResult::invalid_texture_type;
		}

		textureType = GL_TEXTURE_CUBE_MAP;
	}
	else
	{
		if (width == 0)
		{
			return EngineResult::invalid_texture_type;
		}

		textureType = GL_TEXTURE_1D;

		if (height > 0)
		{
			textureType = GL_TEXTURE_2D;

			if (depth > 0)
			{
				textureType = GL_TEXTURE_3D;
			}
		}
		else
		{
			return EngineResult::invalid_texture_type;
		}

		if (arrayElements > 0)
		{
			switch (textureType)
			{
			case GL_TEXTURE_1D:
				textureType = GL_TEXTURE_1D_ARRAY;
				break;
			case GL_TEXTURE_2D:
				textureType = GL_TEXTURE_2D_ARRAY;
				break;
			default:
				return EngineResult::invalid_texture_type;
			}
		}
	}

	if (textureType != GL_TEXTURE_2D)
	{
		return EngineResult::texture_type_mismatch;
	}
		
	std::vector<std::vector<Ceng::UINT8>> imageBuffer;

	std::vector<Ceng::SubResourceData> resourceData;

	Ceng::SubResourceData tempResource;

	Ceng::INT32 imageSize;

	Ceng::INT32 mipLevel, arrayIndex, faceID,depthLevel;

	for (mipLevel = 0; mipLevel < maxMipLevels; ++mipLevel)
	{
		fileReader.read(paramBuffer, 4);
		imageSize = CorrectByteOrder(intValue[0], swapByteOrder);

		tempResource.rowPitch = (width >> mipLevel)*bytesPerPixel;
		tempResource.depthPitch = tempResource.rowPitch*(height >> mipLevel);
		tempResource.compressedSize = imageSize;

		for (arrayIndex = 0; arrayIndex < maxArrayIndex; ++arrayIndex)
		{
			if (textureType == GL_TEXTURE_CUBE_MAP)
			{
				for (faceID = 0; faceID < 6; ++faceID)
				{
					for (depthLevel = 0; depthLevel < maxDepth; ++depthLevel)
					{
						imageBuffer.push_back(std::vector<Ceng::UINT8>(imageSize));

						Ceng::UINT8 *target = &imageBuffer[imageBuffer.size() - 1][0];

						if (swapByteOrder == true)
						{
							for (Ceng::INT32 k = 0; k < imageSize; ++k)
							{
								fileReader.read((char*)&target[k], 1);
							}
						}
						else
						{
							fileReader.read((char*)target, imageSize);
							if (!fileReader.good())
							{
								return EngineResult::io_error;
							}
						}

						tempResource.sourcePtr = target;
						resourceData.push_back(tempResource);
					}

					// Skip cube face padding
					Ceng::INT32 cubePadding = 0;
					if ((imageSize % 4) != 0)
					{
						cubePadding = 4 - (imageSize % 4);
					}

					fileReader.seekg(cubePadding,std::ios::cur);
				}
			}
			else
			{
				for (depthLevel = 0; depthLevel < maxDepth; ++depthLevel)
				{
					imageBuffer.push_back(std::vector<Ceng::UINT8>(imageSize));

					Ceng::UINT8 *target = &imageBuffer[imageBuffer.size() - 1][0];

					if (swapByteOrder == true)
					{
						for (Ceng::INT32 k = 0; k < imageSize; ++k)
						{
							fileReader.read((char*)&target[k], 1);
						}
					}
					else
					{
						fileReader.read((char*)target, imageSize);
						if (!fileReader.good())
						{
							return EngineResult::io_error;
						}
					}

					tempResource.sourcePtr = target;
					resourceData.push_back(tempResource);
				}
			}
		}

		Ceng::INT32 mipPadding = 0;
		if ((imageSize % 4) != 0)
		{
			mipPadding = 4 - (imageSize % 4);
		}

		fileReader.seekg(mipPadding, std::ios::cur);
	}

	Ceng::GL_Texture2dDesc desc;

	desc.width = width;
	desc.height = height;
	desc.mipLevels = maxMipLevels;
	desc.sRGB = options.sRGB;
	desc.gl_format = gl_format;
	desc.gl_internalFormat = gl_internalFormat;
	desc.gl_baseInternalFormat = gl_baseInternalFormat;
	desc.gl_type = gl_datatype;
	desc.bindFlags = options.bindFlags;
	desc.optionFlags = options.options;
	desc.cpuAccessFlags = options.cpuAccessFlags;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;

	Ceng::CRESULT cresult = renderDevice->CreateTexture2D(desc,&resourceData[0], out_texture);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	return EngineResult::ok;
}

