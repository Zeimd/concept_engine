#pragma once

#ifndef CENGINE_BASIC_VERTEX_SHADER_H
#define CENGINE_BASIC_VERTEX_SHADER_H

#include <array>

#include <ceng/datatypes/vshader-input-desc.h>

#include <ceng/interfaces/vshader-instance.h>

#include <ceng/swshader/vshader-input.h>
#include <ceng/swshader/vshader-output.h>

namespace CEngine
{
	class BasicVertexShader : public Ceng::VertexShaderInstance
	{
	public:

		// Pointer to current output fragment. Output registers
		// access this value through a pointer.
		Ceng::POINTER outputBaseAddress;

		Ceng::UINT32 fragmentSizeBytes;

		Ceng::POINTER* inputBaseAddress;
		
		Ceng::POINTER* inputSteps;

		std::array<Ceng::Vshader::VertexShaderInputRegister, 5> inputRegisters;

		Ceng::Vshader::InFloat4 inPosition;
		Ceng::Vshader::InFloat4 inNormal;
		Ceng::Vshader::InFloat4 inTangent;
		Ceng::Vshader::InFloat2 inTexCoord0;
		Ceng::Vshader::InFloat2 inTexCoord1;

		std::array<Ceng::Vshader::VertexShaderOutputRegister, 5> outputRegisters;

		Ceng::Vshader::OutFloat4 outPosition;
		Ceng::Vshader::OutFloat4 outNormal;
		Ceng::Vshader::OutFloat4 outTangent;
		Ceng::Vshader::OutFloat2 outTexCoord0;
		Ceng::Vshader::OutFloat2 outTexCoord1;

		std::array<Ceng::Vshader::VertexShaderUniform, 1> uniforms;

		Ceng::Vshader::UniformMat4x4 fullVertexTransform;

	protected:

		~BasicVertexShader() override
		{

		}


	public:

		BasicVertexShader(Ceng::UINT32 cacheLine, Ceng::Vshader::CR_VertexShaderInput* nullInput,
			Ceng::Vshader::CR_VertexShaderOutput* nullOutput);

		void Release() override;

		Ceng::Vshader::VertexShaderInputRegister* GetInputs() override;
		Ceng::UINT32 InputSize() override;

		Ceng::Vshader::VertexShaderOutputRegister* GetOutputs() override;
		Ceng::UINT32 OutputSize() override;

		Ceng::Vshader::VertexShaderUniform* GetUniforms() override;
		Ceng::UINT32 UniformSize() override;

		Ceng::POINTER* OutputBaseAddress() override;

		Ceng::CRESULT BasicConfig(Ceng::UINT32 fragmentSizeBytes,
			Ceng::POINTER* inputBaseAddress, Ceng::POINTER* inputSteps) override;

		Ceng::CRESULT ProcessVertexBatch(Ceng::UINT32 vertexCount,
			Ceng::FragmentCacheTag* vertexIndex,
			Ceng::UINT8* output,
			Ceng::UINT32 threadId) override;

		void ShaderFunction();
	};
}

#endif