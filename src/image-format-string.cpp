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
#define FORMAT_CASE(x) case Ceng::IMAGE_FORMAT::x: return #x;

	switch (format)
	{
		FORMAT_CASE(UNKNOWN);
		FORMAT_CASE(D16);
		FORMAT_CASE(D16_S8);
		FORMAT_CASE(D16_S16);
		FORMAT_CASE(D24_S8);
		FORMAT_CASE(D24_S16);
		FORMAT_CASE(D32F);
		FORMAT_CASE(D32F_S8);
		FORMAT_CASE(D32F_S16);
		FORMAT_CASE(D32F_IW);
		FORMAT_CASE(D32F_IW_S8);
		FORMAT_CASE(D32F_IW_S16);
		FORMAT_CASE(D32F_W);
		FORMAT_CASE(D32F_W_S8);
		FORMAT_CASE(D32F_W_S16);
		FORMAT_CASE(D64F);
		FORMAT_CASE(D64F_S8);
		FORMAT_CASE(D64F_S16);
		FORMAT_CASE(D64F_IW);
		FORMAT_CASE(D64F_IW_S8);
		FORMAT_CASE(D64F_IW_S16);
		FORMAT_CASE(D64F_W);
		FORMAT_CASE(D64F_W_S8);
		FORMAT_CASE(D64F_W_S16);
		FORMAT_CASE(s8);
		FORMAT_CASE(s16);
		FORMAT_CASE(D32);
		FORMAT_CASE(D15_S1);
		FORMAT_CASE(D24_X8);
		FORMAT_CASE(D24_X4_S4);
		FORMAT_CASE(D24F_S8);
		FORMAT_CASE(unorm_r8);
		FORMAT_CASE(uint_r8);
		FORMAT_CASE(int_r8);
		FORMAT_CASE(unorm_r16);
		FORMAT_CASE(uint_r16);
		FORMAT_CASE(int_r16);
		FORMAT_CASE(fp32_r);
		FORMAT_CASE(fp16_r);
		FORMAT_CASE(fp16_gray);
		FORMAT_CASE(gray_8);
		FORMAT_CASE(gray_16);
		FORMAT_CASE(gray_24);
		FORMAT_CASE(gray_32);
		FORMAT_CASE(alpha_8);
		FORMAT_CASE(unorm_r8_g8);
		FORMAT_CASE(uint_r8_g8);
		FORMAT_CASE(int_r8_g8);
		FORMAT_CASE(unorm_g8_r8);
		FORMAT_CASE(uint_g8_r8);
		FORMAT_CASE(int_g8_r8);
		FORMAT_CASE(unorm_r16_g16);
		FORMAT_CASE(uint_r16_g16);
		FORMAT_CASE(int_r16_g16);
		FORMAT_CASE(unorm_g16_r16);
		FORMAT_CASE(uint_g16_r16);
		FORMAT_CASE(int_g16_r16);
		FORMAT_CASE(fp32_gr);
		FORMAT_CASE(fp32_rg);
		FORMAT_CASE(fp16_gr);
		FORMAT_CASE(fp16_rg);
		FORMAT_CASE(unorm_r8_g8_b8);
		FORMAT_CASE(uint_r8_g8_b8);
		FORMAT_CASE(int_r8_g8_b8);
		FORMAT_CASE(unorm_b8_g8_r8);
		FORMAT_CASE(uint_b8_g8_r8);
		FORMAT_CASE(int_b8_g8_r8);
		FORMAT_CASE(unorm_b32_g32_r32);
		FORMAT_CASE(fp32_bgr);
		FORMAT_CASE(fp32_rgb);
		FORMAT_CASE(unorm_a8_r8_g8_b8);
		FORMAT_CASE(uint_a8_r8_g8_b8);
		FORMAT_CASE(int_a8_r8_g8_b8);
		FORMAT_CASE(unorm_a8_b8_g8_r8);
		FORMAT_CASE(uint_a8_b8_g8_r8);
		FORMAT_CASE(int_a8_b8_g8_r8);
		FORMAT_CASE(unorm_a16_b16_g16_r16);
		FORMAT_CASE(uint_a16_b16_g16_r16);
		FORMAT_CASE(int_a16_b16_g16_r16);
		FORMAT_CASE(unorm_a16_r16_g16_b16);
		FORMAT_CASE(unorm_a32_b32_g32_r32);
		FORMAT_CASE(fp32_abgr);
		FORMAT_CASE(fp32_argb);
		FORMAT_CASE(fp16_abgr);
		FORMAT_CASE(fp16_argb);
		FORMAT_CASE(unorm_r5_g6_b5);
		FORMAT_CASE(unorm_b5_g6_r5);
		FORMAT_CASE(unorm_a1_r5_g5_b5);
		FORMAT_CASE(unorm_a1_b5_g5_r5);
		FORMAT_CASE(unorm_a2_r10_g10_b10);
		FORMAT_CASE(unorm_a2_b10_g10_r10);
		FORMAT_CASE(unorm_r3_g3_b2);
		FORMAT_CASE(unorm_b2_g3_r3);
		FORMAT_CASE(unorm_x8_r8_g8_b8);
		FORMAT_CASE(unorm_x8_b8_g8_r8);
		FORMAT_CASE(unorm_x1_r5_g5_b5);
		FORMAT_CASE(unorm_a4_r4_g4_b4);
		FORMAT_CASE(unorm_a4_b4_g4_r4);
		FORMAT_CASE(unorm_x4_r4_g4_b4);
		FORMAT_CASE(unorm_x4_b4_g4_r4);
		FORMAT_CASE(unorm_a8_r3_g3_b2);
		FORMAT_CASE(unorm_a8_b2_g3_r3);
		FORMAT_CASE(dxt1);
		FORMAT_CASE(dxt2);
		FORMAT_CASE(dxt3);
		FORMAT_CASE(dxt4);
		FORMAT_CASE(dxt5);
		FORMAT_CASE(normal_3dc);
		FORMAT_CASE(bc7_rgba_unorm);	
		FORMAT_CASE(bc7_srgb_alpha_unorm);
		FORMAT_CASE(bc6_rgb_float);
		FORMAT_CASE(bc6_rgb_unsigned_float);
		FORMAT_CASE(etc2_rgb8);
		FORMAT_CASE(etc2_srgb8);
		FORMAT_CASE(etc2_rgb8_a1);
		FORMAT_CASE(etc2_srgb8_a1);
		FORMAT_CASE(etc2_rgba8);
		FORMAT_CASE(etc2_srgb8_a8);
		FORMAT_CASE(eac_r11_unsigned);
		FORMAT_CASE(eac_r11_signed);
		FORMAT_CASE(eac_rg11_unsigned);
		FORMAT_CASE(eac_rg11_signed);
		FORMAT_CASE(etc1_rgb);
		FORMAT_CASE(bc5_rg_unorm);
		FORMAT_CASE(bc5_rg_snorm);
		FORMAT_CASE(bc4_r_unorm);
		FORMAT_CASE(bc4_r_snorm);
	}

#undef FORMAT_CASE

	return "UNHANDLED";
}