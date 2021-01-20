/*****************************************************************************
*
* position-component.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "position-component.h"

#include "rotation-component.h"

using namespace CEngine;


PositionComponent::PositionComponent() : x(0.0f), y(0.0f), z(0.0f), rotation(nullptr)
{
	positionMatrix.Identity();
}

PositionComponent::PositionComponent(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y, const Ceng::FLOAT32 z)
	: x(x),y(y),z(z)
{
	positionMatrix.Identity();
}

PositionComponent::~PositionComponent()
{
}

void PositionComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
	if (name == "rotation")
	{
		rotation = static_cast<RotationComponent*>(component);
	}
}

const Ceng::Matrix4 PositionComponent::ReversePositionMatrix()
{
	positionMatrix.data[0][3] = -x;
	positionMatrix.data[1][3] = -y;
	positionMatrix.data[2][3] = -z;

	return positionMatrix;
}

const Ceng::Matrix4 PositionComponent::PositionMatrix()
{
	positionMatrix.data[0][3] = x;
	positionMatrix.data[1][3] = y;
	positionMatrix.data[2][3] = z;

	return positionMatrix;
}

void PositionComponent::MoveByDelta(const Ceng::VectorF4 &deltaPos)
{
	if (rotation == nullptr)
	{
		x += deltaPos.x;
		y += deltaPos.y;
		z += deltaPos.z;
	}
	else
	{
		x += rotation->rotationBasis[0].x*deltaPos.x +
			rotation->rotationBasis[1].x*deltaPos.y +
			rotation->rotationBasis[2].x*deltaPos.z;

		y += rotation->rotationBasis[0].y*deltaPos.x +
			rotation->rotationBasis[1].y*deltaPos.y +
			rotation->rotationBasis[2].y*deltaPos.z;

		z += rotation->rotationBasis[0].z*deltaPos.x +
			rotation->rotationBasis[1].z*deltaPos.y +
			rotation->rotationBasis[2].z*deltaPos.z;
	}
}

void PositionComponent::MoveByWorldDelta(const Ceng::VectorF4 &deltaPos)
{
	x += deltaPos.x;
	y += deltaPos.y;
	z += deltaPos.z;
}
