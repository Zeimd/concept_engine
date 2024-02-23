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
#include "math-vector.h"

namespace CEngine
{
	const EngineResult::value CreateIrradianceMap(Ceng::Cubemap *envMap,Ceng::Cubemap *irradianceMap);

	void RayDir(const Ceng::UINT32 u, const Ceng::UINT32 v, const Ceng::CubemapFace::value face,
		const Ceng::FLOAT32 cubeWidth, Vec3 *out_dir);

	class CubemapData
	{
	public:
		Ceng::UINT32 width;
		Ceng::UINT32 faceSize;
		Vec4* faceData[6];

		CubemapData()
		{
			faceSize = 0;
			for (int i = 0; i < 6; i++)
			{
				faceData[i] = nullptr;
			}
		}

		CubemapData(Ceng::UINT32 width) : width(width)
		{
			faceSize = 4 * width * width;

			for (int i = 0; i < 6; i++)
			{
				faceData[i] = (Vec4*)malloc(faceSize * sizeof(Vec4));
			}
		}

		~CubemapData()
		{
			for (int i = 0; i < 6; i++)
			{
				if (faceData[i] != nullptr)
				{
					free(faceData[i]);
				}
			}
		}
	};

	struct IrradianceThreadCommon
	{
		Ceng::FLOAT32 invDestWidth;
		Ceng::Matrix4* faceMatrix;
		Ceng::Matrix4* faceTranspose;
		Vec4* solidAngleRayDir;
		CubemapData* sourceMap;
		CubemapData* destMap;
	};

	struct TaskData_v3
	{
		Vec3* normal;
		Vec4 output;
		Vec4* dest;
		Ceng::UINT32 sourceFace;
	};

}

#endif