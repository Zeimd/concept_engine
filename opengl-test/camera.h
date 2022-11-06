/*****************************************************************************
*
* camera.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_CAMERA_H
#define CENGINE_CAMERA_H

#include <ceng.h>

#include "engine-object.h"

namespace CEngine
{
	class Camera : public EngineObject
	{
	protected:

		Ceng::Quaternion reverseRotation;

		Ceng::VectorF4 defaultBasis[3];

	public:
		Camera();
		~Camera() override;

		void RotateByDeltas(const Ceng::FLOAT32 xAngle,
			const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle) override;

		const Ceng::Matrix4 GetRotationMatrix() const override;
		const Ceng::Matrix4 GetPositionMatrix() override;
		const Ceng::Matrix4 GetFullTransformation() override;

		const Ceng::Matrix4 GetReverseRotation() const;
	};
}

#endif
