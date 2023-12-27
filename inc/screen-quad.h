/*****************************************************************************
*
* screen-quad.h
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#ifndef CENGINE_SCREEN_QUAD_H
#define CENGINE_SCREEN_QUAD_H

#include <ceng.h>

#include "engine-result.h"
#include "vertex-formats.h"

namespace CEngine
{
	// Provides the vertex format and data needed to render a full screen quad
	class FullScreenQuad
	{
	public:

		static CEngine::QuadVertex fullScreenQuad[4];

		static Ceng::UINT8 fullScreenIndices[6];

		Ceng::VertexFormat* quadVertexFormat;

		Ceng::VertexBuffer* quadVertexBuffer;

		Ceng::IndexBuffer* quadIndices;

	protected:
		FullScreenQuad();

		FullScreenQuad(Ceng::VertexFormat* quadVertexFormat, Ceng::VertexBuffer* quadVertexBuffer, Ceng::IndexBuffer* quadIndices);

	public:


		~FullScreenQuad();

		static EngineResult::value GetInstance(Ceng::RenderDevice* renderDevice, FullScreenQuad** output);

	};
}

#endif
