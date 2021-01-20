/*****************************************************************************
*
* eng-object.h
*
* By Jari Korkala 10/2011
*
*****************************************************************************/

#ifndef _ENG_OBJECT_H
#define _ENG_OBJECT_H

#include <ceng.h>

#include "shader-manager.h"
#include "shader.h"
#include "shader-program.h"
#include "texture.h"
#include "texture-manager.h"

#include "material-manager.h"
#include "material.h"

#include "mesh.h"
#include "mesh-manager.h"
#include "mesh-action.h"

#include "entity.h"
#include "position-component.h"
#include "rotation-component.h"
#include "mesh-component.h"
#include "point-light-component.h"

#include "point-light-shader.h"
#include "direction-light-shader.h"
#include "depth-pass.h"
#include "shadowmap-component.h"

#include "spot-light-component.h"
#include "spot-light-shader.h"
#include "spot-shadow-shader.h"

#include "math-vector.h"

#include "env-map.h"

namespace CEngine
{
	constexpr Ceng::UINT32 majorVersion = 0;
	constexpr Ceng::UINT32 minorVersion = 1;
	constexpr Ceng::UINT32 build = 113;
}

struct ColorF4
{
	Ceng::FLOAT32 blue;
	Ceng::FLOAT32 green;
	Ceng::FLOAT32 red;
	Ceng::FLOAT32 alpha;
};

struct TexelF4
{
	Ceng::FLOAT32 u;
	Ceng::FLOAT32 v;
	Ceng::FLOAT32 s;
	Ceng::FLOAT32 t;
};

#include "math-vector.h"

struct ProgVertexData
{
	CEngine::Vec3 location;
	CEngine::Vec3 normal;
	CEngine::Vec3 tangent;
	CEngine::Vec3 binormal;

	CEngine::TexelF2 texCoord;
};

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

	void SetWorldPosition(const Ceng::VectorF4 &newPos);
	
	virtual void MoveByDelta(const Ceng::VectorF4 &deltaPos);
	virtual void MoveByWorldDelta(const Ceng::VectorF4 &deltaPos);

	virtual void GetPosition(CEngine::Vec3 *position);
};

struct SurfaceData
{
	Ceng::INT32 vertexIndices[8];
};

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

class Actor : public EngineObject
{
protected:

public:
	Actor();
	virtual ~Actor();

//	Ceng::CRESULT SetMesh(Mesh *meshPtr);
};

const Ceng::String ImageFormatToString(const Ceng::IMAGE_FORMAT::value format);

const Ceng::CRESULT CreateTexture2dFromFile(const Ceng::StringUtf8 &file, Ceng::Texture2dDesc &desc,
	Ceng::RenderDevice *renderDevice, Ceng::Texture2D **out_texture);

namespace TextureError
{
	enum value
	{
		ok,
		invalid_param,
		file_not_found,
		unexpected_eof,
		io_error,
		unknown_file_format,
		invalid_texture_type,
		texture_type_mismatch,
		failed,
		out_of_memory,
	};
}

#include "object-error.h"

#endif