/*****************************************************************************
*
* point-light-component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_LIGHT_COMPONENT_H
#define CENGINE_LIGHT_COMPONENT_H

#include <memory>
#include <ceng.h>
#include "component.h"

namespace CEngine
{
	class PositionComponent;

	class ShaderProgram;

	class PointLightComponent : public Component
	{
	public:
		Ceng::VectorF4 color;
		Ceng::FLOAT32 power;
		Ceng::FLOAT32 cutoffDist;

		PositionComponent *position;

	public:
		PointLightComponent();

		PointLightComponent(const Ceng::VectorF4 &color,const Ceng::FLOAT32 power,const Ceng::FLOAT32 cutoffDist);

		virtual ~PointLightComponent();

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;
		virtual void RemoveComponent(const Ceng::StringUtf8 &name) override;
	};
}

#endif
