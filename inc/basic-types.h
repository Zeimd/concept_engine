/*****************************************************************************
*
* basic-types.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_BASIC_TYPES_H
#define CENGINE_BASIC_TYPES_H

#include <ceng/math/ce-vector.h>

namespace CEngine
{
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
}

#endif
