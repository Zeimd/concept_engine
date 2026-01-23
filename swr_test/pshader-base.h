#pragma once

#ifndef CENG_SWR_PSHADER_BASE
#define CENG_SWR_PSHADER_BASE

#include <ceng/interfaces/pshader-instance.h>

#include <ceng/datatypes/return-val.h>

#include <ceng/datatypes/aligned-buffer.h>

#include "pshader-input.h"
#include "pshader-output.h"

namespace Ceng::SWRender
{
	class PixelShaderBase : public Ceng::PixelShaderInstance
	{
	public:

		AlignedBuffer<CR_PixelShaderInput> inputRegisters;

		POINTER quadTargetStart;

		/**
		 * Temporary buffer for current quad.
		 */
		AlignedBuffer<Ceng::UINT8> quadBuffer;

		// w for each pixel in a quad (4xFLOAT or 4xDOUBLE)
		// must be 16-byte aligned
		AlignedBuffer<Ceng::UINT8> perspectiveTemp;

		/**
		 * Pixel coverage mask for render target writes.
		 * Accessed through pointers.
		 */
		POINTER coverageAddress;

		POINTER inputBaseAddress;

		POINTER stepBufferPtr;

		AlignedBuffer<CR_psOutputRegister> outputRegisters;

		// Semantic links

		// Input references
		CR_PixelShaderInput* IN_POSITION;
		CR_PixelShaderInput* IN_SCREENPOS;

		CR_PixelShaderInput* IN_NORMAL;
		CR_PixelShaderInput* IN_BINORMAL;
		CR_PixelShaderInput* IN_TANGENT;

		CR_PixelShaderInput* IN_COLOR0;
		CR_PixelShaderInput* IN_COLOR1;

		CR_PixelShaderInput* IN_TEXCOORD0;
		CR_PixelShaderInput* IN_TEXCOORD1;
		CR_PixelShaderInput* IN_TEXCOORD2;
		CR_PixelShaderInput* IN_TEXCOORD3;
		CR_PixelShaderInput* IN_TEXCOORD4;
		CR_PixelShaderInput* IN_TEXCOORD5;
		CR_PixelShaderInput* IN_TEXCOORD6;
		CR_PixelShaderInput* IN_TEXCOORD7;

		// Output references

		CR_psOutputRegister* OUT_TARGET0;
		CR_psOutputRegister* OUT_TARGET1;
		CR_psOutputRegister* OUT_TARGET2;
		CR_psOutputRegister* OUT_TARGET3;
		CR_psOutputRegister* OUT_TARGET4;
		CR_psOutputRegister* OUT_TARGET5;
		CR_psOutputRegister* OUT_TARGET6;
		CR_psOutputRegister* OUT_TARGET7;

		CR_psOutputRegister* OUT_DEPTH;
		CR_psOutputRegister* OUT_STENCIL;

		Pshader::SampleTexture2D sample2d;

		TextureUnit diffuseTexUnit;

	public:

		~PixelShaderBase() override
		{

		}

		void Release() override
		{
			delete this;
		}

		virtual CRESULT ShaderFunction(const FLOAT32* perspective,
										const FLOAT32* invertW,
										const INT32 coverageIndex,
										const Ceng::INT32 threadId) = 0;
	};
}

#endif