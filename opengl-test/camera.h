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
		virtual ~Camera();

		virtual void RotateByDeltas(const Ceng::FLOAT32 xAngle,
			const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle) override;

		virtual const Ceng::Matrix4 GetRotationMatrix() const override;
		virtual const Ceng::Matrix4 GetPositionMatrix() override;
		virtual const Ceng::Matrix4 GetFullTransformation() override;

		const Ceng::Matrix4 GetReverseRotation() const;
	};

	class FPSCamera : public Camera
	{
	protected:

		Ceng::VectorF4 walkDir;

	public:
		FPSCamera();
		virtual ~FPSCamera();

		virtual void RotateByDeltas(const Ceng::FLOAT32 xAngle,
			const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle) override;

		//virtual void MoveByDelta(const Ceng::VectorF4 &deltaPos) override;

		/*
		virtual const Ceng::Matrix4 GetRotationMatrix() const override;
		virtual const Ceng::Matrix4 GetPositionMatrix() override;
		virtual const Ceng::Matrix4 GetFullTransformation() override;

		const Ceng::Matrix4 GetReverseRotation() const;
		*/
	};

}

#endif
