/*****************************************************************************
*
* render-params.h
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#ifndef CENGINE_RENDER_PARAMS_H
#define CENGINE_RENDER_PARAMS_H

#include <ceng.h>

#include "math-vector.h"

namespace CEngine
{
	struct DeferredPassCommonParams
	{
		Ceng::UINT32 windowWidth;
		Ceng::UINT32 windowHeight;

		Ceng::FLOAT32 xDilationDiv;
		Ceng::FLOAT32 yDilationDiv;

		Ceng::FLOAT32 zTermA;
		Ceng::FLOAT32 zTermB;

		// Texture slot for G-Buffer color and material
		Ceng::UINT32 gbufferColorSlot;

		// Texture slot for G-Buffer normals
		Ceng::UINT32 gbufferNormalSlot;

		// Texture slot for G-Buffer depth
		Ceng::UINT32 depthBufferSlot;
	};

	struct EnvMapCommonParams
	{
		// Texture slot for environment cube map
		Ceng::UINT32 envMapSlot;

		// Texture slot for diffuse irradiance
		Ceng::UINT32 irradianceSlot;

		// For rotating eye space vectors to world space
		Ceng::Matrix4 cameraReverseRotation;

		// For translating eye space vectors to world space
		CEngine::Vec3 cameraWorldPos;
	};
}

#endif
