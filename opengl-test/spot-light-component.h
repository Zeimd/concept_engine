/*****************************************************************************
*
* spot-light-component.h
*
* By Jari Korkala 11/6/2015
*
*****************************************************************************/

#ifndef CENGINE_SPOT_LIGHT_COMPONENT_H
#define CENGINE_SPOT_LIGHT_COMPONENT_H

#include <ceng.h>
#include "component.h"

namespace CEngine
{
	class SpotLightComponent : public Component
	{
	public:
		Ceng::FLOAT32 outerAngle;
		Ceng::FLOAT32 innerAngle;

		Ceng::FLOAT32 outerCos;
		Ceng::FLOAT32 innerCos;

	protected:
		SpotLightComponent();
	public:
		~SpotLightComponent() override;

		std::shared_ptr<Component> Clone() const override;

		SpotLightComponent(const Ceng::FLOAT32 outerAngle,const Ceng::FLOAT32 innerAngle);

		void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;
	};
};

#endif