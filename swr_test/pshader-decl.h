#pragma once

#ifndef CENG_SWR_PSHADER_DECL_H
#define CENG_SWR_PSHADER_DECL_H

#include <vector>

#include <ceng/interfaces/pshader-descriptor.h>

#include "pshader-base.h"

namespace Ceng::SWRender
{
	template<class T>
	class PixelShaderDecl : public Ceng::PixelShaderDescriptor
	{
	public:

		std::vector<PixelShaderInputDesc> inputs;

		std::vector<PixelShaderOutputDesc> outputs;

		std::vector<ShaderUniformDesc> uniforms;

	public:

		~PixelShaderDecl() override
		{

		}

		Ceng::UINT32 InputAmount() override
		{
			return inputs.size();
		}

		PixelShaderInputDesc* InputArray() override
		{
			return &inputs[0];
		}

		Ceng::UINT32 UniformAmount() override
		{
			return uniforms.size();
		}

		ShaderUniformDesc* UniformArray() override
		{
			return &uniforms[0];
		}

		Ceng::UINT32 OutputAmount() override
		{
			return outputs.size();
		}

		PixelShaderOutputDesc* OutputArray() override
		{
			return &outputs[0];
		}

		PixelShaderInstance* GetInstance() override
		{
			return new T();
		}
	};
}

#endif