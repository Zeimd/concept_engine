/*****************************************************************************
*
* image-format-string.cpp
*
* Created By Jari Korkala 2/3/2015
*
*****************************************************************************/

#include <ceng.h>

#include "image-format-string.h"

const Ceng::String CEngine::ImageFormatToString(const Ceng::IMAGE_FORMAT::value format)
{
	switch (format)
	{
	case Ceng::IMAGE_FORMAT::unorm_r5_g6_b5:
		return "unorm_r5_g6_b5";
	case Ceng::IMAGE_FORMAT::unorm_a8_b8_g8_r8:
		return "unorm_a8_b8_g8_r8";
	case Ceng::IMAGE_FORMAT::unorm_x8_b8_g8_r8:
		return "unorm_x8_b8_g8_r8";

	case Ceng::IMAGE_FORMAT::unorm_a8_r8_g8_b8:
		return "unorm_a8_r8_g8_b8";
	case Ceng::IMAGE_FORMAT::unorm_x8_r8_g8_b8:
		return "unorm_x8_r8_g8_b8";

	case Ceng::IMAGE_FORMAT::unorm_a2_b10_g10_r10:
		return "unorm_a2_b10_g10_r10";
	case Ceng::IMAGE_FORMAT::unorm_a2_r10_g10_b10:
		return "unorm_a2_r10_g10_b10";
	}

	return "UNHANDLED";
}