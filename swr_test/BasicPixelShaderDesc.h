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

		void Release() override;

		Ceng::UINT32 InputAmount() override;

		const Ceng::PixelShaderInputDesc* InputArray() override;

		Ceng::UINT32 UniformAmount() override;

		const Ceng::ShaderUniformDesc* UniformArray() override;

		Ceng::UINT32 OutputAmount() override;

		const Ceng::PixelShaderOutputDesc* OutputArray() override;

		Ceng::PixelShaderInstance* GetInstance() override;

	};
}

#endif