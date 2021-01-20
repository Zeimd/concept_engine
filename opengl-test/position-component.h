/*****************************************************************************
*
* position-component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_POSITION_COMPONENT_H
#define CENGINE_POSITION_COMPONENT_H

#include <ceng.h>

#include "component.h"

namespace CEngine
{
	class RotationComponent;

	class PositionComponent : public Component
	{
	public:
		Ceng::FLOAT32 x;
		Ceng::FLOAT32 y;
		Ceng::FLOAT32 z;

		Ceng::Matrix4 positionMatrix;

		RotationComponent *rotation;
	public:

		PositionComponent();

		PositionComponent(const Ceng::FLOAT32 x,const Ceng::FLOAT32 y,const Ceng::FLOAT32 z);

		virtual ~PositionComponent();

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component);

		void MoveByDelta(const Ceng::VectorF4 &deltaPos);
		void MoveByWorldDelta(const Ceng::VectorF4 &deltaPos);

		const Ceng::Matrix4 PositionMatrix();

		const Ceng::Matrix4 ReversePositionMatrix();
	};
}

#endif