#pragma once

#ifndef CENGINE_BASIC_VERTEX_SHADER_DESC_H
#define CENGINE_BASIC_VERTEX_SHADER_DESC_H

#include <ceng/interfaces/vshader-descriptor.h>

namespace CEngine
{
	class BasicVertexShaderDesc : public Ceng::VertexShaderDescriptor
	{
	protected:

		~BasicVertexShaderDesc() override;

	public:

		BasicVertexShaderDesc();

		void Release() override;


		Ceng::UINT32 InputAmount() override;

		const Ceng::VertexShaderInputDesc* InputArray() override;

		Ceng::UINT32 UniformAmount() override;

		const Ceng::ShaderUniformDesc* UniformArray() override;

		Ceng::UINT32 OutputAmount() override;

		const Ceng::VertexShaderOutputDesc* OutputArray() override;

		Ceng::VertexShaderInstance* GetInstance(Ceng::UINT32 cacheLine, Ceng::Vshader::CR_VertexShaderInput* nullInput,
			Ceng::Vshader::CR_VertexShaderOutput* nullOutput) override;
	};
}

#endif