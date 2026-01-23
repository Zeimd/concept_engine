#pragma once

#ifndef CENGINE_BASIC_PIXEL_SHADER_H
#define CENGINE_BASIC_PIXEL_SHADER_H

#include <vector>

#include <ceng/datatypes.h>
#include <ceng/interfaces/pshader-instance.h>

#include <ceng/datatypes/aligned-buffer.h>

#include "pshader-input.h"
#include "pshader-output.h"

using namespace Ceng;

namespace CEngine
{
	class BasicPixelShader : public Ceng::PixelShaderInstance
	{
	protected:

		~BasicPixelShader() override;

	public:		

		// Output format

		AlignedBuffer<SWRender::CR_PixelShaderInput> inputRegisters;

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

		AlignedBuffer<SWRender::CR_psOutputRegister> outputRegisters;

		// Semantic links

		// Input references
		SWRender::CR_PixelShaderInput* IN_POSITION;
		SWRender::CR_PixelShaderInput* IN_SCREENPOS;

		SWRender::CR_PixelShaderInput* IN_NORMAL;
		SWRender::CR_PixelShaderInput* IN_BINORMAL;
		SWRender::CR_PixelShaderInput* IN_TANGENT;

		SWRender::CR_PixelShaderInput* IN_COLOR0;
		SWRender::CR_PixelShaderInput* IN_COLOR1;

		SWRender::CR_PixelShaderInput* IN_TEXCOORD0;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD1;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD2;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD3;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD4;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD5;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD6;
		SWRender::CR_PixelShaderInput* IN_TEXCOORD7;

		// Output references

		SWRender::CR_psOutputRegister* OUT_TARGET0;
		SWRender::CR_psOutputRegister* OUT_TARGET1;
		SWRender::CR_psOutputRegister* OUT_TARGET2;
		SWRender::CR_psOutputRegister* OUT_TARGET3;
		SWRender::CR_psOutputRegister* OUT_TARGET4;
		SWRender::CR_psOutputRegister* OUT_TARGET5;
		SWRender::CR_psOutputRegister* OUT_TARGET6;
		SWRender::CR_psOutputRegister* OUT_TARGET7;

		SWRender::CR_psOutputRegister* OUT_DEPTH;
		SWRender::CR_psOutputRegister* OUT_STENCIL;

		Pshader::SampleTexture2D sample2d;

		TextureUnit diffuseTexUnit;

	public:

		BasicPixelShader();

		void Release() override;

		CRESULT ConfigureInput(PixelShaderInputDesc* inputs, Ceng::UINT32 amount) override;

		CRESULT ConfigureOutput(PixelShaderOutputDesc* outputs, Ceng::UINT32 amount) override;

		CRESULT SetFragmentFormat(const PixelShaderInputDesc* inputs, Ceng::UINT32 inputAmount,
			const PixelShaderOutputDesc* outputs, Ceng::UINT32 outputAmount) override;

		CRESULT SetRenderTargets(const PixelShaderOutputDesc* outputs, Ceng::UINT32 amount) override;

		CRESULT ProcessQuads(QuadData* quads, Ceng::UINT32 amount) override;

	public:

	

	public:

	};
}

#endif