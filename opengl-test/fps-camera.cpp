/*****************************************************************************
*
* fps-camera.cpp
*
* By Jari Korkala 16/12/2015
*
*****************************************************************************/

#include "engine-v2.h"

using namespace CEngine;


FPSCamera::FPSCamera() : Camera(), pitchAngle(0), maxPitch(85.0f)
{
}

FPSCamera::FPSCamera(Ceng::FLOAT32 maxPitch) : Camera(), maxPitch(maxPitch)
{

}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::RotateByDeltas(const Ceng::FLOAT32 xAngle,
	const Ceng::FLOAT32 yAngle,
	const Ceng::FLOAT32 zAngle)
{
	Ceng::Quaternion temp;
	Ceng::Quaternion reverse;

	Ceng::FLOAT32 xAngleFinal = xAngle;

	if (pitchAngle + xAngle > maxPitch)
	{
		xAngleFinal = maxPitch - pitchAngle;
	}
	else if (pitchAngle + xAngle < -maxPitch)
	{
		xAngleFinal = -maxPitch - pitchAngle;
	}

	pitchAngle += xAngleFinal;

	// Create rotation around x-axis
	temp.CreateRotation(defaultBasis[0], -xAngleFinal);
	reverse.CreateRotation(rotationBasis[0], xAngle);

	// rotationBasis[] is used to determine local movement directions,
	// so its y-component must remain (0,1,0) for correct translations.

	// Rotate default y axis so that view transformations are correct
	temp.RotatePoint(&defaultBasis[1], &defaultBasis[1], 1);

	// Update rotation data
	rotation.AddRotation(temp);
	reverseRotation.AddRotation(reverse);

	// Do internal rotation around the modified y axis
	temp.CreateRotation(defaultBasis[1], -yAngle);

	// Do camera rotation around default y axis
	reverse.CreateRotation(Ceng::VectorF4(0.0f, 1.0f, 0.0f), yAngle);

	// Rotate translation basis so that movement on the xz-plane 
	// is correct
	reverse.RotatePoint(&rotationBasis[0], &rotationBasis[0], 1);
	reverse.RotatePoint(&rotationBasis[2], &rotationBasis[2], 1);

	rotation.AddRotation(temp);
	reverseRotation.AddRotation(reverse);
}