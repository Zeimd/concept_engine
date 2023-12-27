/*****************************************************************************
*
* mesh-component.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "mesh-component.h"

#include "position-component.h"
#include "rotation-component.h"

#include "mesh.h"

using namespace CEngine;

MeshComponent::MeshComponent()
	: position(&defaultPos), rotation(&defaultRot)
{
}

MeshComponent::MeshComponent(const std::shared_ptr<Mesh> &mesh) 
	: mesh(mesh), position(&defaultPos), rotation(&defaultRot)
{
}

MeshComponent::~MeshComponent()
{
}

std::shared_ptr<Component> MeshComponent::Clone() const
{
	return std::make_shared<MeshComponent>(this->mesh);
}


void MeshComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
	if (name == "position")
	{
		position = static_cast<PositionComponent*>(component);
	}
	else if (name == "rotation")
	{
		rotation = static_cast<RotationComponent*>(component);
	}
}

void MeshComponent::RemoveComponent(const Ceng::StringUtf8& name)
{
	if (name == "position")
	{
		position = &defaultPos;
	}
	else if (name == "rotation")
	{
		rotation = &defaultRot;
	}
}

void MeshComponent::Render(Ceng::RenderContext *context, Ceng::Matrix4 *cameraTransform,
	Ceng::Matrix4 *cameraRotation, Ceng::Matrix4 *projection, Ceng::SamplerState *materialSampler)
{
	if (mesh == nullptr) return;

	Ceng::Matrix4 objectRotation(rotation->RotationMatrix());

	objectTransform = position->PositionMatrix() * objectRotation;

	Ceng::Matrix4 fullTransform = *projection * *cameraTransform * objectTransform;

	Ceng::Matrix4 normalRotation = *cameraRotation * objectRotation;

	mesh->Render(context, &fullTransform, &normalRotation, materialSampler);
}

void MeshComponent::ShadowRender(Ceng::RenderContext *context, Ceng::Matrix4 *cameraTransform, 
	Ceng::Matrix4 *projection, Ceng::ShaderConstant *u_fullTransform)
{
	if (mesh == nullptr) return;

	Ceng::Matrix4 objectRotation(rotation->RotationMatrix());

	objectTransform = position->PositionMatrix() * objectRotation;

	Ceng::Matrix4 fullTransform = *projection * *cameraTransform * objectTransform;

	u_fullTransform->SetMatrix_4x4(&fullTransform.data[0][0], true);

	mesh->ShadowRender(context, &fullTransform);
}