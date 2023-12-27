/*****************************************************************************
*
* env-map.h
*
* By Jari Korkala 25/11/2015
*
*****************************************************************************/

#ifndef CENGINE_ENV_MAP_H
#define CENGINE_ENV_MAP_H

#include <ceng.h>

#include "engine-result.h"

namespace CEngine
{
	const EngineResult::value CreateIrradianceMap(Ceng::Cubemap *envMap,Ceng::Cubemap *irradianceMap);

	void RayDir(const Ceng::UINT32 u, const Ceng::UINT32 v, const Ceng::CubemapFace::value face,
		const Ceng::FLOAT32 cubeWidth, Ceng::FLOAT32 *out_dir);
}

#endif