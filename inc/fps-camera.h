/*****************************************************************************
*
* fps-camera.h
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#ifndef CENGINE_FPS_CAMERA_H
#define CENGINE_FPS_CAMERA_H

#include <ceng.h>

#include "camera.h"

namespace CEngine
{
	class FPSCamera : public Camera
	{
	protected:

		Ceng::VectorF4 walkDir;

		Ceng::FLOAT32 pitchAngle;

		Ceng::FLOAT32 maxPitch;

	public:
		FPSCamera();

		FPSCamera(Ceng::FLOAT32 maxPitch);

		~FPSCamera() override;

		void RotateByDeltas(const Ceng::FLOAT32 xAngle,
			const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle) override;
	};
}



#endif