/*****************************************************************************
*
* vertex-formats.h
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#ifndef CENGINE_VERTEX_FORMATS_H
#define CENGINE_VERTEX_FORMATS_H

#include "math-vector.h"

namespace CEngine
{
	struct QuadVertex
	{
		CEngine::Vec2 position;
		CEngine::TexelF2 texCoord;
	};
}

#endif
