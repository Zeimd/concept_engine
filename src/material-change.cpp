/*****************************************************************************
*
* material-change.cpp
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#include "mesh.h"
#include "mesh-action.h"

using namespace CEngine;

MaterialChange::MaterialChange()
{
}

MaterialChange::MaterialChange(std::shared_ptr<Material> &material) : material(material)
{
}

MaterialChange::~MaterialChange()
{
}

void MaterialChange::Execute(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
	Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler)
{
	material->Apply(context, fullTransform, normalTransform, materialSampler);
}