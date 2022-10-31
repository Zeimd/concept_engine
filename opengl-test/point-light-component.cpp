/*****************************************************************************
*
* light-component.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "position-component.h"
#include "point-light-component.h"
#include "shader-program.h"

using namespace CEngine;

PointLightComponent::PointLightComponent()
{
}

PointLightComponent::PointLightComponent(const Ceng::VectorF4 &color, const Ceng::FLOAT32 power, const Ceng::FLOAT32 cutoffDist)
	: color(color),power(power),cutoffDist(cutoffDist)
{
}

PointLightComponent::~PointLightComponent()
{
}

std::shared_ptr<Component> PointLightComponent::Clone() const
{
	return std::make_shared< PointLightComponent>(color, power, cutoffDist);
}

void PointLightComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
	if (name == "position")
	{
		position = static_cast<PositionComponent*>(component);
	}
}

void PointLightComponent::RemoveComponent(const Ceng::StringUtf8 &name)
{
}


