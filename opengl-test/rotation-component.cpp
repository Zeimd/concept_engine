/*****************************************************************************
*
* rotation-component.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "rotation-component.h"

using namespace CEngine;

RotationComponent::RotationComponent()
{
	rotation.Identity();

	rotationBasis[0] = Ceng::VectorF4(Ceng::FLOAT32(1.0f),
		Ceng::FLOAT32(0.0f),
		Ceng::FLOAT32(0.0f));

	rotationBasis[1] = Ceng::VectorF4(Ceng::FLOAT32(0.0f),
		Ceng::FLOAT32(1.0f),
		Ceng::FLOAT32(0.0f));

	rotationBasis[2] = Ceng::VectorF4(Ceng::FLOAT32(0.0f),
		Ceng::FLOAT32(0.0f),
		Ceng::FLOAT32(1.0f));
}

RotationComponent::~RotationComponent()
{
}


void RotationComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
}

void RotationComponent::RotateByDeltas(const Ceng::FLOAT32 xAngle, const Ceng::FLOAT32 yAngle,
	const Ceng::FLOAT32 zAngle)
{
	Ceng::Quaternion temp;

	// Create rotation around x-axis
	temp.CreateRotation(rotationBasis[0], xAngle);

	// Rotate y and z axes
	temp.RotatePoint(&rotationBasis[1], &rotationBasis[1], 2);

	// Update rotation data
	rotation.AddRotation(temp);

	// Create rotation around y-axis
	temp.CreateRotation(rotationBasis[1], yAngle);

	// Rotate x and z axes
	temp.RotatePoint(&rotationBasis[0], &rotationBasis[0], 1);
	temp.RotatePoint(&rotationBasis[2], &rotationBasis[2], 1);

	rotation.AddRotation(temp);

	// Create rotation around z-axis
	temp.CreateRotation(rotationBasis[2], zAngle);

	// Rotate x and y axes
	temp.RotatePoint(&rotationBasis[0], &rotationBasis[0], 2);

	rotation.AddRotation(temp);
}

const Ceng::Matrix4 RotationComponent::RotationMatrix() const
{
	return Ceng::Matrix4(rotation);
}