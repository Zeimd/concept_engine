#include <array>

#include "BasicPixelShaderDesc.h"
#include "BasicPixelShader.h"

#include <ceng/enums/shader-datatype.h>
#include <ceng/enums/shader-semantic.h>

using namespace CEngine;

static const std::array<Ceng::PixelShaderInputDesc, 5> inputs
{ {
	{
		Ceng::SHADER_DATATYPE::FLOAT4,
		"normal",
		Ceng::SHADER_SEMANTIC::NORMAL
	},
	{
		Ceng::SHADER_DATATYPE::FLOAT4,
		"tangent",
		Ceng::SHADER_SEMANTIC::TANGENT
	},
	{
		Ceng::SHADER_DATATYPE::FLOAT2,
		"texCoord0",
		Ceng::SHADER_SEMANTIC::TEXCOORD_0
	},
	{
		Ceng::SHADER_DATATYPE::FLOAT2,
		"texCoord1",
		Ceng::SHADER_SEMANTIC::TEXCOORD_1
	},
	{
		Ceng::SHADER_DATATYPE::FLOAT2,
		"texCoord2",
		Ceng::SHADER_SEMANTIC::TEXCOORD_2
	},
} };

static const std::array<Ceng::ShaderUniformDesc, 1> uniforms
{ {
	{
		Ceng::SHADER_DATATYPE::sampler2d,
		"diffuseTexture"
	}
} };

static const std::array<Ceng::PixelShaderOutputDesc, 1> outputs
{ {
	{
		Ceng::PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET0,
	}
} };

BasicPixelShaderDesc::BasicPixelShaderDesc()
{

}

BasicPixelShaderDesc::~BasicPixelShaderDesc()
{

}

void BasicPixelShaderDesc::Release()
{
	delete this;
}

Ceng::UINT32 BasicPixelShaderDesc::InputAmount()
{
	return inputs.size();
}

const Ceng::PixelShaderInputDesc* BasicPixelShaderDesc::InputArray()
{
	if (inputs.size() == 0)
	{
		return nullptr;
	}

	return &inputs[0];
}

Ceng::UINT32 BasicPixelShaderDesc::UniformAmount()
{
	return uniforms.size();
}

const Ceng::ShaderUniformDesc* BasicPixelShaderDesc::UniformArray()
{
	if (uniforms.size() == 0)
	{
		return nullptr;
	}

	return &uniforms[0];
}

Ceng::UINT32 BasicPixelShaderDesc::OutputAmount()
{
	return outputs.size();
}

const Ceng::PixelShaderOutputDesc* BasicPixelShaderDesc::OutputArray()
{
	if (outputs.size() == 0)
	{
		return nullptr;
	}

	return &outputs[0];
}

Ceng::PixelShaderInstance* BasicPixelShaderDesc::GetInstance(Ceng::UINT32 cacheLine, Pshader::CR_PixelShaderInput* nullInput)
{
	return new BasicPixelShader(cacheLine, nullInput);
}