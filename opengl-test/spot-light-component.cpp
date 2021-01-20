/*****************************************************************************
*
* spot-light-component.cpp
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#include "spot-light-component.h"

using namespace CEngine;

SpotLightComponent::SpotLightComponent()
{
}

SpotLightComponent::~SpotLightComponent()
{
}

SpotLightComponent::SpotLightComponent(const Ceng::FLOAT32 outerAngle, const Ceng::FLOAT32 innerAngle)
	: outerAngle(outerAngle),innerAngle(innerAngle)
{
	outerCos = cos(0.5f * outerAngle * Ceng::degree_to_radians);
	innerCos = cos(0.5f * innerAngle * Ceng::degree_to_radians);
}

void SpotLightComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
}