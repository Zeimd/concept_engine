#include "BasicPixelShaderDesc.h"

#include <ceng/enums/shader-datatype.h>
#include <ceng/enums/shader-semantic.h>

using namespace CEngine;

BasicPixelShaderDesc::BasicPixelShaderDesc()
{
	Ceng::PixelShaderInputDesc psTempRegister;

	psTempRegister.semantic = Ceng::SHADER_SEMANTIC::NORMAL;
	inputs.push_back(psTempRegister);

	psTempRegister.semantic = Ceng::SHADER_SEMANTIC::TANGENT;
	inputs.push_back(psTempRegister);

	psTempRegister.semantic = Ceng::SHADER_SEMANTIC::TEXCOORD_0;
	inputs.push_back(psTempRegister);

	psTempRegister.semantic = Ceng::SHADER_SEMANTIC::TEXCOORD_1;
	inputs.push_back(psTempRegister);

	Ceng::ShaderUniformDesc tempConst;

	tempConst.dataType = Ceng::SHADER_DATATYPE::UINT;
	tempConst.name = "diffuseTexture";

	uniforms.push_back(tempConst);

	Ceng::PixelShaderOutputDesc psTempTarget;

	psTempTarget.target = Ceng::PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET0;
	outputs.push_back(psTempTarget);
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

Ceng::PixelShaderInputDesc* BasicPixelShaderDesc::InputArray()
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

Ceng::ShaderUniformDesc* BasicPixelShaderDesc::UniformArray()
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

Ceng::PixelShaderOutputDesc* BasicPixelShaderDesc::OutputArray()
{
	if (outputs.size() == 0)
	{
		return nullptr;
	}

	return &outputs[0];
}

Ceng::PixelShaderInstance* BasicPixelShaderDesc::GetInstance()
{
	return nullptr;
}