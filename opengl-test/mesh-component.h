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

namespace CEngine
{
	class PositionComponent;
	class RotationComponent;

	class Mesh;

	class MeshComponent : public Component
	{
	public:
		Ceng::Matrix4 objectTransform;

		std::shared_ptr<Mesh> mesh;

		PositionComponent *position;
		RotationComponent *rotation;

	public:
		MeshComponent();

		MeshComponent(const std::shared_ptr<Mesh> &mesh);

		virtual ~MeshComponent();

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;

		void Render(Ceng::RenderContext *context, Ceng::Matrix4 *cameraTransform,
			Ceng::Matrix4 *cameraRotation, Ceng::Matrix4 *projection,Ceng::SamplerState *materialSampler);

		void ShadowRender(Ceng::RenderContext *context, Ceng::Matrix4 *cameraTransform,Ceng::Matrix4 *projection,
			Ceng::ShaderConstant *u_fullTransform);
	};
}

#endif
