/*****************************************************************************
*
* rotation-component.h
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#ifndef CENGINE_ROTATION_COMPONENT_H
#define CENGINE_ROTATION_COMPONENT_H

#include <ceng.h>
#include "component.h"

namespace CEngine
{
	namespace EulerMode
	{
		enum value
		{
			xyz = 0,
			yzx = 1,
			zxy = 2,
			xzy = 3,
			zyx = 4,
			yxz = 5,

			zxz = 6,
			xyx = 7,
			yzy = 8,
			zyz = 9,
			xzx = 10,
			yxy = 11,

			force32b = uint32_t(1) << 31,
		};
	}
	
	class RotationComponent : public Component
	{
	public:
		Ceng::Quaternion rotation;
		Ceng::VectorF4 rotationBasis[3];


	public:

		RotationComponent();

		// Initialize with Euler angles
		RotationComponent(EulerMode::value mode, Ceng::FLOAT32 a, 
			Ceng::FLOAT32 b, Ceng::FLOAT32 c);

		// Initialize from quaternion
		RotationComponent(Ceng::FLOAT32 w, Ceng::FLOAT32 x, 
			Ceng::FLOAT32 y, Ceng::FLOAT32 z);

		virtual ~RotationComponent();

		virtual void AddComponent(const Ceng::StringUtf8 &name, Component *component) override;

		// Set to identity rotation
		void SetIdentity();

		/*
		void RotateByDeltas(const Ceng::FLOAT32 xAngle, const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle);
			*/

		void SetEulerAngles(EulerMode::value mode, Ceng::FLOAT32 a,
			Ceng::FLOAT32 b, Ceng::FLOAT32 c);

		void SetQuaternion(Ceng::FLOAT32 w, Ceng::FLOAT32 x,
			Ceng::FLOAT32 y, Ceng::FLOAT32 z);

		void SetQuaternion(const Ceng::Quaternion &q);

		void AddRotation(const Ceng::Quaternion& q);

		void AddRotation(EulerMode::value mode, Ceng::FLOAT32 a,
			Ceng::FLOAT32 b, Ceng::FLOAT32 c);

		const Ceng::Matrix4 RotationMatrix() const;
	};
}

#endif
