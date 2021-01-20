/*****************************************************************************
*
* eng-camera.cpp
*
* By Jari Korkala 10/2011
*
*****************************************************************************/

#include "engine-v2.h"

Camera::Camera()
{
	reverseRotation.Identity();
	defaultBasis[0] = rotationBasis[0];
	defaultBasis[1] = rotationBasis[1];
	defaultBasis[2] = rotationBasis[2];
}

Camera::~Camera()
{
}

void Camera::RotateByDeltas(const Ceng::FLOAT32 xAngle, 
							const Ceng::FLOAT32 yAngle, 
							const Ceng::FLOAT32 zAngle)
{
	Ceng::Quaternion temp;
	Ceng::Quaternion reverse;

	// Create rotation around x-axis
	temp.CreateRotation(defaultBasis[0],-xAngle);
	reverse.CreateRotation(rotationBasis[0],xAngle);

	// Rotate y and z axis
	reverse.RotatePoint(&rotationBasis[1],&rotationBasis[1],2);

	// Update rotation data
	rotation.AddRotation(temp);
	reverseRotation.AddRotation(reverse);

	temp.CreateRotation(defaultBasis[1],-yAngle);
	reverse.CreateRotation(rotationBasis[1],yAngle);

	reverse.RotatePoint(&rotationBasis[0],&rotationBasis[0],1);
	reverse.RotatePoint(&rotationBasis[2],&rotationBasis[2],1);

	rotation.AddRotation(temp);
	reverseRotation.AddRotation(reverse);

	temp.CreateRotation(defaultBasis[2],-zAngle);
	reverse.CreateRotation(rotationBasis[2],zAngle);

	reverse.RotatePoint(&rotationBasis[0],&rotationBasis[0],2);

	rotation.AddRotation(temp);
	reverseRotation.AddRotation(reverse);

}

const Ceng::Matrix4 Camera::GetFullTransformation()
{
	positionMatrix.data[0][3] = -position.x;
	positionMatrix.data[1][3] = -position.y;
	positionMatrix.data[2][3] = -position.z;

	return Ceng::Matrix4(rotation) * positionMatrix;
}

const Ceng::Matrix4 Camera::GetPositionMatrix()
{
	positionMatrix.data[0][3] = -position.x;
	positionMatrix.data[1][3] = -position.y;
	positionMatrix.data[2][3] = -position.z;

	return positionMatrix;
}

const Ceng::Matrix4 Camera::GetRotationMatrix() const
{
	return Ceng::Matrix4(rotation);
}

const Ceng::Matrix4 Camera::GetReverseRotation() const
{
	Ceng::Quaternion temp = rotation;
	temp.Reciprocal();
	return Ceng::Matrix4(temp);
}