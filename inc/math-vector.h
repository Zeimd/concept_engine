/*****************************************************************************
*
* math-vector.h
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#ifndef CENGINE_MATH_VECTOR_H
#define CENGINE_MATH_VECTOR_H

#include <ceng/datatypes.h>

namespace CEngine
{
	struct TexelF2
	{
		Ceng::FLOAT32 u;
		Ceng::FLOAT32 v;
	};

	struct Vec2
	{
		Ceng::FLOAT32 x;
		Ceng::FLOAT32 y;
	};

	struct Vec3
	{
		Ceng::FLOAT32 x;
		Ceng::FLOAT32 y;
		Ceng::FLOAT32 z;
	};

	struct Vec4
	{
		Ceng::FLOAT32 x;
		Ceng::FLOAT32 y;
		Ceng::FLOAT32 z;
		Ceng::FLOAT32 w;
	};

	struct BasisVec3
	{
		Vec3 forward;
		Vec3 right;
		Vec3 up;
	};

	struct BasisVec4
	{
		Vec4 forward;
		Vec4 right;
		Vec4 up;
	};

}

#endif
