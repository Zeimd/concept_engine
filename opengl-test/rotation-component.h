/*****************************************************************************
*
* rotation-component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_ROTATION_COMPONENT_H
#define CENGINE_ROTATION_COMPONENT_H

#include <ceng.h>
#include "component.h"

namespace CEngine
{
	class RotationComponent : public Component
	{
	public:
		Ceng::Quaternion rotation;
		Ceng::VectorF4 rotationBasis[3];

	public:

		RotationComponent();

		virtual ~RotationComponent();

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;

		virtual void RotateByDeltas(const Ceng::FLOAT32 xAngle, const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle);

		virtual const Ceng::Matrix4 RotationMatrix() const;
	};
}

#endif
