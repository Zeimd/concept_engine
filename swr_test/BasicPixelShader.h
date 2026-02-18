#pragma once

#ifndef CENGINE_BASIC_PIXEL_SHADER_H
#define CENGINE_BASIC_PIXEL_SHADER_H

#include <array>

#include <ceng/datatypes.h>
#include <ceng/interfaces/pshader-instance.h>

#include <ceng/datatypes/aligned-buffer.h>

#include <ceng/swshader/pshader-input.h>
#include <ceng/swshader/pshader-output.h>

using namespace Ceng;

namespace CEngine
{
	class BasicPixelShader : public Ceng::PixelShaderInstance
	{
	protected:

		~BasicPixelShader() override;

	public:		
		Pshader::RenderTargetService* renderTargetService;

		Ceng::UINT32 instanceIndex;

		POINTER quadTargetStart;

		UINT32 quadFloatOffset; 
		UINT32 quadDoubleOffset; 
		UINT32 quadTargetOffset; 

		UINT32 floatBlockSize;
		UINT32 doubleBlockSize;

		SWRender::CR_FloatFragment* floatParam;
		SWRender::CR_DoubleFragment* doubleParam;

		Ceng::VectorF4 trianglePackedW;
		Ceng::VectorF4 trianglePackedW_dx;
		Ceng::VectorF4 trianglePackedW_dy;

		UINT8* variableStep;

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
		Ceng::UINT32 coverageAddress;

		POINTER inputBaseAddress;

		POINTER stepBufferPtr;

		// Add input variables in the same order they appear in PixelShaderDeclaration.
		std::array<Ceng::Pshader::PixelShaderInputRegister, 4> inputRegisters;

		Pshader::InFloat4 normal;
		Pshader::InFloat4 tangent;
		Pshader::InFloat2 texCoord0;
		Pshader::InFloat2 texCoord1;

		std::array<Ceng::Pshader::PixelShaderUniform, 1> uniforms;

		Pshader::UniformSampler2d diffuseTex;

		std::array<Ceng::Pshader::PixelShaderOutputRegister, 10> outputRegisters;

		Pshader::OutFloat OUT_STENCIL;
		Pshader::OutFloat OUT_DEPTH;

		Pshader::OutFloat4 OUT_TARGET0;
		Pshader::OutFloat4 OUT_TARGET1;
		Pshader::OutFloat4 OUT_TARGET2;
		Pshader::OutFloat4 OUT_TARGET3;
		Pshader::OutFloat4 OUT_TARGET4;
		Pshader::OutFloat4 OUT_TARGET5;
		Pshader::OutFloat4 OUT_TARGET6;
		Pshader::OutFloat4 OUT_TARGET7;

	public:

		BasicPixelShader(Ceng::UINT32 cacheLine, Pshader::CR_PixelShaderInput* nullInput);

		void Release() override;

		CRESULT BasicConfig(Ceng::UINT32 quadSize, Ceng::UINT32 cacheLine, Ceng::POINTER quadTargetStart) override;

		Pshader::PixelShaderInputRegister* GetInputs() override;
		Ceng::UINT32 InputSize() override;

		Pshader::PixelShaderOutputRegister* GetOutputs() override;
		Ceng::UINT32 OutputSize() override;

		Pshader::PixelShaderUniform* GetUniforms() override;
		Ceng::UINT32 UniformSize() override;

		Ceng::UINT8* QuadBuffer() override;

		Ceng::POINTER* StepBufferPtr() override;

		void* PerpectiveTemp() override;

		Ceng::UINT32* CoverageAddress() override;

		void SetRenderTargetService(Pshader::RenderTargetService* service) override;

		void ProcessConfig(
			UINT32 quadFloatOffset,
			UINT32 quadDoubleOffset,
			UINT32 quadTargetOffset,
			UINT32 floatBlockSize,
			UINT32 doubleBlockSize) override;

		CRESULT ProcessQuads(SWRender::PixelShaderQuadBatch* batch, Ceng::UINT32 batchSize, 
			SWRender::PixelShaderTriangleData* triangleData, Ceng::UINT32 threadId) override;

	public:

		void ShaderFunction(const FLOAT32* perspective, const FLOAT32* invertW,
			const Ceng::INT32 coverage, const Ceng::INT32 threadId);
	

	public:

	};
}

#endif