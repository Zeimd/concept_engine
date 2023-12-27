/*****************************************************************************
*
* mesh.cpp
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#include "mesh.h"
#include "mesh-action.h"

using namespace CEngine;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	for (auto &buffer : indexBuffers)
	{
		buffer->Release();
	}

	for (auto &buffer : vertexBuffers)
	{
		buffer->Release();
	}
}

void Mesh::Render(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform,
	Ceng::Matrix4 *normalTransform, Ceng::SamplerState *materialSampler)
{
	for (auto &command : commands)
	{
		command->Execute(context, fullTransform, normalTransform, materialSampler);
	}
}

void Mesh::ShadowRender(Ceng::RenderContext *context, Ceng::Matrix4 *fullTransform)
{
	for (auto &command : shadowCommands)
	{
		command->Execute(context, nullptr,nullptr,nullptr);
	}
}

