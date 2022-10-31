/*****************************************************************************
*
* rotation-component.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "rotation-component.h"

using namespace CEngine;

constexpr Ceng::UINT32 eulerAxisOrder[][3] =
{
	{0,1,2}, // XYZ
	{1,2,0}, // YZX
	{2,0,1}, // ZXY
	{0,2,1}, // XZY
	{2,1,0}, // ZYX
	{1,0,2}, // YXZ

	{2,0,2}, // XZX
	{0,1,0}, // XYX
	{1,2,1}, // YZY
	{2,1,2}, // ZYZ
	{0,2,0}, // XZX
	{1,0,1}, // YXY

};

void RotationComponent::AddRotation(const Ceng::Quaternion& q)
{
	q.RotatePoint(rotationBasis, rotationBasis, 3);

	rotation.AddRotation(q);
}

void RotationComponent::AddRotation(EulerMode::value mode, Ceng::FLOAT32 a,
	Ceng::FLOAT32 b, Ceng::FLOAT32 c)
{
	Ceng::FLOAT32 angles[] = { a,b,c };

	const Ceng::UINT32* order = &eulerAxisOrder[mode][0];

	Ceng::Quaternion temp;

	for (int step = 0; step < 3; ++step)
	{
		// Create rotation around x-axis
		temp.CreateRotation(rotationBasis[order[step]], angles[step]);

		// Rotate other two axes
		for (int k = 0; k < 3; k++)
		{
			if (k != order[step])
			{
				temp.RotatePoint(&rotationBasis[k], &rotationBasis[k], 1);
			}
		}

		rotation.AddRotation(temp);
	}
}


void RotationComponent::SetIdentity()
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

void RotationComponent::SetQuaternion(Ceng::FLOAT32 w, Ceng::FLOAT32 x,
	Ceng::FLOAT32 y, Ceng::FLOAT32 z)
{
	SetIdentity();

	Ceng::Quaternion temp(w, x, y, z);

	AddRotation(temp);
}

void RotationComponent::SetQuaternion(const Ceng::Quaternion& q)
{
	SetIdentity();

	AddRotation(q);
}

void RotationComponent::SetEulerAngles(EulerMode::value mode, Ceng::FLOAT32 a,
	Ceng::FLOAT32 b, Ceng::FLOAT32 c)
{
	SetIdentity();

	AddRotation(mode, a, b, c);
}

RotationComponent::RotationComponent()
{
	SetIdentity();
}

RotationComponent::RotationComponent(EulerMode::value mode, Ceng::FLOAT32 a, 
	Ceng::FLOAT32 b, Ceng::FLOAT32 c)
{
	SetEulerAngles(mode, a, b, c);
}

RotationComponent::RotationComponent(Ceng::FLOAT32 w, Ceng::FLOAT32 x, 
	Ceng::FLOAT32 y, Ceng::FLOAT32 z) : RotationComponent()
{
	SetQuaternion(w, x, y, z);
}

RotationComponent::RotationComponent(const Ceng::Quaternion& q)
{
	SetQuaternion(q);
}

RotationComponent::~RotationComponent()
{
}

std::shared_ptr<Component> RotationComponent::Clone() const
{
	auto temp = std::make_shared<RotationComponent>();

	// Less work than initialization from quaternion

	temp->rotation = rotation;
	temp->rotationBasis[0] = rotationBasis[0];
	temp->rotationBasis[1] = rotationBasis[1];
	temp->rotationBasis[2] = rotationBasis[2];

	return temp;
}



void RotationComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
}

/*
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
*/

const Ceng::Matrix4 RotationComponent::RotationMatrix() const
{
	return Ceng::Matrix4(rotation);
}