/*****************************************************************************
*
* eng-object.cpp
*
* By Jari Korkala 10/2011
*
*****************************************************************************/

#include "engine-v2.h"

EngineObject::EngineObject()
{
	position = Ceng::VectorF4();

	rotation.Identity();

	positionMatrix.Identity();

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

EngineObject::~EngineObject()
{
}

void EngineObject::GetPosition(CEngine::Vec3 *out_position)
{
	out_position->x = position.x;
	out_position->y = position.y;
	out_position->z = position.z;
}

const Ceng::Matrix4 EngineObject::GetRotationMatrix() const
{
	return Ceng::Matrix4(rotation);
}

const Ceng::Matrix4 EngineObject::GetPositionMatrix()
{
	positionMatrix.data[0][3] = position.x;
	positionMatrix.data[1][3] = position.y;
	positionMatrix.data[2][3] = position.z;

	return positionMatrix;
}

const Ceng::Matrix4 EngineObject::GetFullTransformation()
{
	positionMatrix.data[0][3] = position.x;
	positionMatrix.data[1][3] = position.y;
	positionMatrix.data[2][3] = position.z;

	return positionMatrix * Ceng::Matrix4(rotation);
}

void EngineObject::SetWorldPosition(const Ceng::VectorF4 &newPos)
{
	position.x = newPos.x;
	position.y = newPos.y;
	position.z = newPos.z;
}

void EngineObject::MoveByDelta(const Ceng::VectorF4 &deltaPos)
{
	position.x += rotationBasis[0].x*deltaPos.x +
					rotationBasis[1].x*deltaPos.y +
					rotationBasis[2].x*deltaPos.z;

	position.y += rotationBasis[0].y*deltaPos.x +
					rotationBasis[1].y*deltaPos.y +
					rotationBasis[2].y*deltaPos.z;

	position.z += rotationBasis[0].z*deltaPos.x +
					rotationBasis[1].z*deltaPos.y +
					rotationBasis[2].z*deltaPos.z;
}

void EngineObject::MoveByWorldDelta(const Ceng::VectorF4 &deltaPos)
{
	position.x += deltaPos.x;
	position.y += deltaPos.y;
	position.z += deltaPos.z;
}

void EngineObject::RotateByDeltas(const Ceng::FLOAT32 xAngle, 
								  const Ceng::FLOAT32 yAngle, 
								  const Ceng::FLOAT32 zAngle)
{
	Ceng::Quaternion temp;

	// Create rotation around x-axis
	temp.CreateRotation(rotationBasis[0],xAngle);

	// Rotate y and z axes
	temp.RotatePoint(&rotationBasis[1],&rotationBasis[1],2);

	// Update rotation data
	rotation.AddRotation(temp);

	// Create rotation around y-axis
	temp.CreateRotation(rotationBasis[1],yAngle);

	// Rotate x and z axes
	temp.RotatePoint(&rotationBasis[0],&rotationBasis[0],1);
	temp.RotatePoint(&rotationBasis[2],&rotationBasis[2],1);

	rotation.AddRotation(temp);

	// Create rotation around z-axis
	temp.CreateRotation(rotationBasis[2],zAngle);

	// Rotate x and y axes
	temp.RotatePoint(&rotationBasis[0],&rotationBasis[0],2);

	rotation.AddRotation(temp);
}



