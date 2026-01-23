#ifndef CENG_SWR_PSHADER_OUTPUT_H
#define CENG_SWR_PSHADER_OUTPUT_H

#include <ceng/math/ce-vector.h>
#include <ceng/enums/image-formats.h>

#include "pshader-types.h"

namespace Ceng::SWRender
{
	const FLOAT32 colorScaleScalar = FLOAT32(255.0f);

	_declspec(align(64)) const Ceng::INT8 coverageTable8[16][4] =
	{
		{ 0, 0, 0, 0 },
		{ -1, 0, 0, 0 },
		{ 0, -1, 0, 0 },
		{ -1, -1, 0, 0 },
		{ 0, 0, -1, 0 },
		{ -1, 0, -1, 0 },
		{ 0, -1, -1, 0 },
		{ -1, -1, -1, 0 },
		{ 0, 0, 0, -1 },
		{ -1, 0, 0, -1 },
		{ 0, -1, 0, -1 },
		{ -1, -1, 0, -1 },
		{ 0, 0, -1, -1 },
		{ -1, 0, -1, -1 },
		{ 0, -1, -1, -1 },
		{ -1, -1, -1, -1 }
	};

	class CR_psOutputRegister
	{
	public:

		/**
		 * A CE_BUFFER_TYPE value.
		 */
		UINT32 bufferFormat;

		/**
		 * Location of render target address.
		 */
		POINTER inputAddress;

		POINTER* coverageAddress;

	public:
		CR_psOutputRegister()
		{
			// Default to a format that doesn't write to a
			// render target

			bufferFormat = Ceng::IMAGE_FORMAT::UNKNOWN;
		}

		~CR_psOutputRegister()
		{
		}

		inline CR_psOutputRegister& operator = (const Pshader::Float& source)
		{
			POINTER* localWrite = (POINTER*)(inputAddress);

			(*call_from_Float[bufferFormat])((void*)(*localWrite), (void*)&source.x,
				(void*)(*coverageAddress));

			*localWrite += 16;
			return *this;
		}

		inline CR_psOutputRegister& operator = (const Pshader::Float4& source)
		{
			POINTER* localWrite = (POINTER*)(inputAddress);

			/*
			(*call_from_Float4[bufferFormat]) ( (void*)(*localWrite),(void*)source.dataAddress,
													(void*)(*coverageAddress));
													*/


			float* dest = (float*)(*localWrite);

			const INT8* coverage = &coverageTable8[(INT32)(*coverageAddress)][0];

			__m128 colorScaleVec = _mm_load1_ps(&colorScaleScalar);

			float* sourcePtr = (float*)&source._x;

			__m128 blueChannel = _mm_load_ps(&sourcePtr[0]);
			__m128 greenChannel = _mm_load_ps(&sourcePtr[4]);
			__m128 redChannel = _mm_load_ps(&sourcePtr[8]);
			__m128 alphaChannel = _mm_load_ps(&sourcePtr[12]);

			blueChannel = _mm_mul_ps(blueChannel, colorScaleVec);
			greenChannel = _mm_mul_ps(greenChannel, colorScaleVec);
			redChannel = _mm_mul_ps(redChannel, colorScaleVec);
			alphaChannel = _mm_mul_ps(alphaChannel, colorScaleVec);

			__m128i blueInt = _mm_cvtps_epi32(blueChannel);
			__m128i greenInt = _mm_cvtps_epi32(greenChannel);
			__m128i redInt = _mm_cvtps_epi32(redChannel);
			__m128i alphaInt = _mm_cvtps_epi32(alphaChannel);

			__m128i br_Word = _mm_packs_epi32(blueInt, redInt);
			__m128i ga_Word = _mm_packs_epi32(greenInt, alphaInt);

			__m128i writeVec = _mm_packs_epi16(br_Word, ga_Word);

			__m128 coverageVecF = _mm_load1_ps((float*)coverage);

			__m128i* coverageVec = (__m128i*) & coverageVecF;

			__m128i destVec = _mm_load_si128((__m128i*)dest);

			// Select pixels from render target that won't be overwritten
			destVec = _mm_andnot_si128(*coverageVec, destVec);

			// Select pixels from input that will be written
			writeVec = _mm_and_si128(*coverageVec, writeVec);

			// Combine pixels
			writeVec = _mm_or_si128(writeVec, destVec);

			_mm_store_si128((__m128i*)dest, writeVec);

			// Step quad chain's target address to next quad on the right
			*localWrite += 16;
			return *this;
		}

		inline void Write(const Ceng::VectorF4& source, const Ceng::INT32 coverageIndex)
		{
			POINTER* localWrite = (POINTER*)(inputAddress);

			/*
			(*call_from_Float[bufferFormat])((void*)(*localWrite), (void*)source,
				(void*)(*coverageAddress));
				*/

			const INT8* coverage = &coverageTable8[coverageIndex][0];

			_declspec(align(16)) Ceng::UINT8 writeBuffer[16];

			// blue
			writeBuffer[0] = Ceng::UINT8(255.0f * source.x);
			writeBuffer[1] = Ceng::UINT8(255.0f * source.x);
			writeBuffer[2] = Ceng::UINT8(255.0f * source.x);
			writeBuffer[3] = Ceng::UINT8(255.0f * source.x);

			// green
			writeBuffer[4] = Ceng::UINT8(255.0f * source.y);
			writeBuffer[5] = Ceng::UINT8(255.0f * source.y);
			writeBuffer[6] = Ceng::UINT8(255.0f * source.y);
			writeBuffer[7] = Ceng::UINT8(255.0f * source.y);

			// red
			writeBuffer[8] = Ceng::UINT8(255.0f * source.z);
			writeBuffer[9] = Ceng::UINT8(255.0f * source.z);
			writeBuffer[10] = Ceng::UINT8(255.0f * source.z);
			writeBuffer[11] = Ceng::UINT8(255.0f * source.z);

			writeBuffer[12] = Ceng::UINT8(255.0f * source.w);
			writeBuffer[13] = Ceng::UINT8(255.0f * source.w);
			writeBuffer[14] = Ceng::UINT8(255.0f * source.w);
			writeBuffer[15] = Ceng::UINT8(255.0f * source.w);

			// Source is ubyte4

			//float *sourcePtr = writeBuffer;
			float* dest = (float*)(*localWrite);

			__m128i writeVec;

			__m128* writeVecF = (__m128*) & writeVec;

			*writeVecF = _mm_load_ps((float*)writeBuffer);

			__m128 coverageVecF = _mm_load1_ps((float*)coverage);

			__m128i* coverageVec = (__m128i*) & coverageVecF;

			__m128i destVec = _mm_load_si128((__m128i*)dest);

			// Select pixels from render target that won't be overwritten
			destVec = _mm_andnot_si128(*coverageVec, destVec);

			// Select pixels from input that will be written
			writeVec = _mm_and_si128(*coverageVec, writeVec);

			// Combine pixels
			writeVec = _mm_or_si128(writeVec, destVec);

			//_mm_adds_epu8(writeVec, destVec);

			_mm_store_si128((__m128i*)dest, writeVec);


			*localWrite += 16;

		};

		inline void Write(const Pshader::SampleTexture2D& source, const Ceng::INT32 coverageIndex)
		{
			_declspec(align(16)) Ceng::FLOAT32 writeBuffer[16];

			source.SampleToFloat4(writeBuffer);

			POINTER* localWrite = (POINTER*)(inputAddress);

			float* sourcePtr = writeBuffer;
			float* dest = (float*)(*localWrite);

			const INT8* coverage = &coverageTable8[coverageIndex][0];

			// Source is ubyte4

			__m128i writeVec;

			__m128* writeVecF = (__m128*) & writeVec;

			*writeVecF = _mm_load_ps(sourcePtr);

			__m128 coverageVecF = _mm_load1_ps((float*)coverage);

			__m128i* coverageVec = (__m128i*) & coverageVecF;

			__m128i destVec = _mm_load_si128((__m128i*)dest);

			// Select pixels from render target that won't be overwritten
			destVec = _mm_andnot_si128(*coverageVec, destVec);

			// Select pixels from input that will be written
			writeVec = _mm_and_si128(*coverageVec, writeVec);

			// Combine pixels
			writeVec = _mm_or_si128(writeVec, destVec);

			_mm_store_si128((__m128i*)dest, writeVec);

			/*

			// Source is float4

			__m128 colorScaleVec = _mm_load1_ps(&colorScaleScalar);

			__m128 blueChannel = _mm_load_ps(&sourcePtr[0]);
			__m128 greenChannel = _mm_load_ps(&sourcePtr[4]);
			__m128 redChannel = _mm_load_ps(&sourcePtr[8]);
			__m128 alphaChannel = _mm_load_ps(&sourcePtr[12]);

			blueChannel = _mm_mul_ps(blueChannel, colorScaleVec);
			greenChannel = _mm_mul_ps(greenChannel, colorScaleVec);
			redChannel = _mm_mul_ps(redChannel, colorScaleVec);
			alphaChannel = _mm_mul_ps(alphaChannel, colorScaleVec);

			__m128i blueInt = _mm_cvtps_epi32(blueChannel);
			__m128i greenInt = _mm_cvtps_epi32(greenChannel);
			__m128i redInt = _mm_cvtps_epi32(redChannel);
			__m128i alphaInt = _mm_cvtps_epi32(alphaChannel);

			__m128i br_Word = _mm_packs_epi32(blueInt, redInt);
			__m128i ga_Word = _mm_packs_epi32(greenInt, alphaInt);

			__m128i writeVec = _mm_packus_epi16(br_Word, ga_Word);

			__m128 coverageVecF = _mm_load1_ps((float*)coverage);

			__m128i *coverageVec = (__m128i*)&coverageVecF;

			__m128i destVec = _mm_load_si128((__m128i*)dest);

			// Select pixels from render target that won't be overwritten
			destVec = _mm_andnot_si128(*coverageVec, destVec);

			// Select pixels from input that will be written
			writeVec = _mm_and_si128(*coverageVec, writeVec);

			// Combine pixels
			writeVec = _mm_or_si128(writeVec, destVec);

			_mm_store_si128((__m128i*)dest, writeVec);
			*/

			// Step quad chain's target address to next quad on the right
			*localWrite += 16;
		}

		inline CR_psOutputRegister& operator = (const Pshader::SampleTexture2D& source)
		{
			POINTER* localWrite = (POINTER*)(inputAddress);

			/*
			(*call_from_Float4[bufferFormat]) ((void*)(*localWrite), (void*)source.dataAddress,
				(void*)(*coverageAddress));
				*/

			float* dest = (float*)(*localWrite);

			const INT8* coverage = &coverageTable8[(INT32)(*coverageAddress)][0];

			__m128 colorScaleVec = _mm_load1_ps(&colorScaleScalar);

			float* sourcePtr = (float*)source.dataAddress;

			__m128 blueChannel = _mm_load_ps(&sourcePtr[0]);
			__m128 greenChannel = _mm_load_ps(&sourcePtr[4]);
			__m128 redChannel = _mm_load_ps(&sourcePtr[8]);
			__m128 alphaChannel = _mm_load_ps(&sourcePtr[12]);

			blueChannel = _mm_mul_ps(blueChannel, colorScaleVec);
			greenChannel = _mm_mul_ps(greenChannel, colorScaleVec);
			redChannel = _mm_mul_ps(redChannel, colorScaleVec);
			alphaChannel = _mm_mul_ps(alphaChannel, colorScaleVec);

			__m128i blueInt = _mm_cvtps_epi32(blueChannel);
			__m128i greenInt = _mm_cvtps_epi32(greenChannel);
			__m128i redInt = _mm_cvtps_epi32(redChannel);
			__m128i alphaInt = _mm_cvtps_epi32(alphaChannel);

			__m128i br_Word = _mm_packs_epi32(blueInt, redInt);
			__m128i ga_Word = _mm_packs_epi32(greenInt, alphaInt);

			__m128i writeVec = _mm_packs_epi16(br_Word, ga_Word);

			__m128 coverageVecF = _mm_load1_ps((float*)coverage);

			__m128i* coverageVec = (__m128i*) & coverageVecF;

			__m128i destVec = _mm_load_si128((__m128i*)dest);

			// Select pixels from render target that won't be overwritten
			destVec = _mm_andnot_si128(*coverageVec, destVec);

			// Select pixels from input that will be written
			writeVec = _mm_and_si128(*coverageVec, writeVec);

			// Combine pixels
			writeVec = _mm_or_si128(writeVec, destVec);

			_mm_store_si128((__m128i*)dest, writeVec);

			// Step quad chain's target address to next quad on the right
			*localWrite += 16;
			return *this;
		}

	public:

		static void (*call_from_Float[128])(void* dest, void* source, void* coverageMask);
		static void (*call_from_Float4[128])(void* dest, void* source, void* coverageMask);
	};

	void PSOUT_NULL(void* dest, void* source, void* coverageMask);

	void PSOUT_C32_ARGB_FLOAT_X86_SSE2(void* dest, void* source, void* coverageMask);
	void PSOUT_C32_ARGB_FLOAT4_X86_SSE2(void* dest, void* source, void* coverageMask);
};

#endif