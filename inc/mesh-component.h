/*****************************************************************************
*
* mesh-component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_MESH_COMPONENT_H
#define CENGINE_MESH_COMPONENT_H

#include <memory>

#include <ceng.h>
#include "component.h"

#include "position-component.h"
#include "rotation-component.h"

namespace CEngine
{

	class Mesh;

	class MeshComponent : public Component
	{
	public:
		Ceng::Matrix4 objectTransform;

		std::shared_ptr<Mesh> mesh;

		PositionComponent defaultPos;
		RotationComponent defaultRot;

		PositionComponent *position;
		RotationComponent *rotation;

	protected:
		MeshComponent();

	public:
		
		MeshComponent(const std::shared_ptr<Mesh> &mesh);

		~MeshComponent() override;

		void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;

		void RemoveComponent(const Ceng::StringUtf8& name) override;

		std::shared_ptr<Component> Clone() const override;

		void Render(Ceng::RenderContext *context, Ceng::Matrix4 *cameraTransform,
			Ceng::Matrix4 *cameraRotation, Ceng::Matrix4 *projection,Ceng::SamplerState *materialSampler);

		void ShadowRender(Ceng::RenderContext *context, Ceng::Matrix4 *cameraTransform,Ceng::Matrix4 *projection,
			Ceng::ShaderConstant *u_fullTransform);
	};
}

#endif
