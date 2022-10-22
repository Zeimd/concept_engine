/*****************************************************************************
*
* engine-object.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_ENGINE_OBJECT_H
#define CENGINE_ENGINE_OBJECT_H

#include <ceng.h>

#include "math-vector.h"

namespace CEngine
{
	class EngineObject
	{
	protected:
		Ceng::VectorF4 position;

		Ceng::Quaternion rotation;
		Ceng::VectorF4 rotationBasis[3];

		Ceng::Matrix4 positionMatrix;
	public:
		EngineObject();
		virtual ~EngineObject();

		virtual void RotateByDeltas(const Ceng::FLOAT32 xAngle,
			const Ceng::FLOAT32 yAngle,
			const Ceng::FLOAT32 zAngle);

		virtual const Ceng::Matrix4 GetRotationMatrix() const;
		virtual const Ceng::Matrix4 GetPositionMatrix();
		virtual const Ceng::Matrix4 GetFullTransformation();

		void SetWorldPosition(const Ceng::VectorF4& newPos);

		virtual void MoveByDelta(const Ceng::VectorF4& deltaPos);
		virtual void MoveByWorldDelta(const Ceng::VectorF4& deltaPos);

		virtual void GetPosition(CEngine::Vec3* position);
	};
}

#endif
