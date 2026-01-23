#pragma once

#ifndef CENGINE_BASIC_PIXEL_SHADER_DESC_H
#define CENGINE_BASIC_PIXEL_SHADER_DESC_H

#include <ceng/datatypes.h>
#include <ceng/interfaces/pshader-descriptor.h>

namespace CEngine
{
	class BasicPixelShaderDesc : public Ceng::PixelShaderDescriptor
	{
	protected:

		~BasicPixelShaderDesc() override;

	public:

		BasicPixelShaderDesc();

		std::vector< Ceng::PixelShaderInputDesc > inputs;
		std::vector< Ceng::PixelShaderOutputDesc > outputs;
		std::vector< Ceng::ShaderUniformDesc > uniforms;

		void Release() override;

		Ceng::UINT32 InputAmount() override;

		Ceng::PixelShaderInputDesc* InputArray() override;

		Ceng::UINT32 UniformAmount() override;

		Ceng::ShaderUniformDesc* UniformArray() override;

		Ceng::UINT32 OutputAmount() override;

		Ceng::PixelShaderOutputDesc* OutputArray() override;

		Ceng::PixelShaderInstance* GetInstance() override;

	};
}

#endif