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

	void RayDir(const Ceng::UINT32 u, const Ceng::UINT32 v, const Ceng::CubemapFace::value face,
		const Ceng::FLOAT32 cubeWidth, Vec4* out_dir);

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
			faceSize = width * width;

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

		// Fill with zeroes
		void Fill()
		{
			return Fill({ 0.0f,0.0f,0.0f,0.0f });
		}

		// Fill with value
		void Fill(const Vec4& value)
		{
			for (Ceng::UINT32 i = 0; i < 6; i++)
			{
				for (Ceng::UINT32 j = 0; j < faceSize; j++)
				{
					faceData[i][j] = value;
				}
			}
		}
	};

	struct IrradianceThreadCommon
	{
		Ceng::FLOAT32 invDestWidth;
		Ceng::FLOAT32 invSourceWidth;
		Ceng::Matrix4* faceMatrix;
		Ceng::Matrix4* faceTranspose;
		
		Vec4* solidAngleRayDir;
		Vec4* solidAngleRayDirQuadrant;

		Ceng::FLOAT32* solidAngleOnly;
		Ceng::FLOAT32* solidAngleOnlyQuadrant;

		Ceng::UINT32 quadrantWidth;


		CubemapData* sourceMap;
		CubemapData* destMap;
	};

	struct TaskData_v3
	{
		Vec4* normal;
		Vec4 output;
		Vec4* dest;
		Ceng::UINT32 sourceFace;
	};

	void PrecalculateNormals(IrradianceThreadCommon& common, Vec4** out_normals);

	void PrecalculateNormalsPosFace(IrradianceThreadCommon& common, Vec4** out_normals);
}

#endif