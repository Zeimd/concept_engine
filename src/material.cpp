/*****************************************************************************
*
* material.cpp
*
* By Jari Korkala 6/6/2016
*
*****************************************************************************/

#include "material.h"

using namespace CEngine;

Material::Material()
{
}

Material::Material(const std::shared_ptr<ShaderProgram> &program, 
	std::vector<std::unique_ptr<MaterialParam>> &&params,
	Ceng::ShaderConstant *vsFullTransform, Ceng::ShaderConstant *vsObjectRotation)
	: program(program),params(std::move(params)),vsFullTransform(vsFullTransform),vsObjectRotation(vsObjectRotation)
{
}

Material::~Material()
{
	vsFullTransform->Release();
	vsObjectRotation->Release();
}

void Material::Apply(Ceng::RenderContext *context, const Ceng::Matrix4 *fullTransform,
	const Ceng::Matrix4 *objectRotation, Ceng::SamplerState *textureSampler)
{
	Ceng::ShaderProgram *progHandle = program->GetProgram();

	context->SetShaderProgram(progHandle);

	vsFullTransform->SetMatrix_4x4(&(*fullTransform).data[0][0], true);
	vsObjectRotation->SetMatrix_4x4(&(*objectRotation).data[0][0], true);

	for (auto &param : params)
	{
		param->Apply(context,textureSampler);
	}
};
