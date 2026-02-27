#include <array>

#include <ceng/enums/shader-datatype.h>
#include <ceng/enums/shader-semantic.h>

#include "BasicVertexShaderDesc.h"

using namespace CEngine;

#include "BasicVertexShader.h"

using namespace CEngine;

static const std::array<Ceng::VertexShaderInputDesc, 5> inputs
{ {
	{
		Ceng::SHADER_SEMANTIC::POSITION
	},
	{
		Ceng::SHADER_SEMANTIC::TANGENT
	},
	{
		Ceng::SHADER_SEMANTIC::TEXCOORD_0
	},
	{
		Ceng::SHADER_SEMANTIC::TEXCOORD_1
	},
	{
		Ceng::SHADER_SEMANTIC::TEXCOORD_2
	},
} };

static const std::array<Ceng::ShaderUniformDesc, 1> uniforms
{ {
	{
		Ceng::SHADER_DATATYPE::FLOAT4x4,
		"fullVertexTransform"
	}
} };

static const std::array<Ceng::VertexShaderOutputDesc, 5> outputs
{ {
	{
		Ceng::SHADER_SEMANTIC::POSITION,
		Ceng::SHADER_DATATYPE::FLOAT4
	},
	{
		Ceng::SHADER_SEMANTIC::NORMAL,
		Ceng::SHADER_DATATYPE::FLOAT4
	},
	{
		Ceng::SHADER_SEMANTIC::TANGENT,
		Ceng::SHADER_DATATYPE::FLOAT4
	},
	{
		Ceng::SHADER_SEMANTIC::TEXCOORD_0,
		Ceng::SHADER_DATATYPE::FLOAT2
	},
	{
		Ceng::SHADER_SEMANTIC::TEXCOORD_1,
		Ceng::SHADER_DATATYPE::FLOAT2
	}
} };

BasicVertexShaderDesc::BasicVertexShaderDesc()
{

}

BasicVertexShaderDesc::~BasicVertexShaderDesc()
{

}

void BasicVertexShaderDesc::Release()
{
	delete this;
}

Ceng::UINT32 BasicVertexShaderDesc::InputAmount()
{
	return inputs.size();
}

const Ceng::VertexShaderInputDesc* BasicVertexShaderDesc::InputArray()
{
	if (inputs.size() == 0)
	{
		return nullptr;
	}

	return &inputs[0];
}

Ceng::UINT32 BasicVertexShaderDesc::UniformAmount()
{
	return uniforms.size();
}

const Ceng::ShaderUniformDesc* BasicVertexShaderDesc::UniformArray()
{
	if (uniforms.size() == 0)
	{
		return nullptr;
	}

	return &uniforms[0];
}

Ceng::UINT32 BasicVertexShaderDesc::OutputAmount()
{
	return outputs.size();
}

const Ceng::VertexShaderOutputDesc* BasicVertexShaderDesc::OutputArray()
{
	if (outputs.size() == 0)
	{
		return nullptr;
	}

	return &outputs[0];
}

Ceng::VertexShaderInstance* BasicVertexShaderDesc::GetInstance(Ceng::UINT32 cacheLine, Ceng::Vshader::CR_VertexShaderInput* nullInput,
	Ceng::Vshader::CR_VertexShaderOutput* nullOutput)
{
	return new BasicVertexShader(cacheLine, nullInput, nullOutput);
}