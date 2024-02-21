/*****************************************************************************
*
* env-map.cpp
*
* By Jari Korkala 25/11/2015
*
*****************************************************************************/

#include <ceng/lib/timerlib.h>

#include <thread>

#include "env-map.h"

using namespace CEngine;

const Ceng::FLOAT32 degrees_to_radians = 0.01745f;

// From AMD Cubemapgen
// https://code.google.com/archive/p/cubemapgen/
const Ceng::FLOAT32 AreaElement(const Ceng::FLOAT32 x, const Ceng::FLOAT32 y)
{
	return atan2(x * y, sqrt(x * x + y * y + 1));
}

// From AMD Cubemapgen
// https://code.google.com/archive/p/cubemapgen/
const Ceng::FLOAT32 TexelCoordSolidAngle(const Ceng::INT32 a_FaceIdx, const Ceng::FLOAT32 a_U, 
	const Ceng::FLOAT32 a_V, const Ceng::INT32 a_Size)
{
	// transform from [0..res - 1] to [- (1 - 1 / res) .. (1 - 1 / res)]
	// (+ 0.5f is for texel center addressing)
	Ceng::FLOAT32 U = (2.0f * ((Ceng::FLOAT32)a_U + 0.5f) / (Ceng::FLOAT32)a_Size) - 1.0f;
	Ceng::FLOAT32 V = (2.0f * ((Ceng::FLOAT32)a_V + 0.5f) / (Ceng::FLOAT32)a_Size) - 1.0f;

	// Shift from a demi texel, mean 1.0f / a_Size with U and V in [-1..1]
	Ceng::FLOAT32 InvResolution = 1.0f / a_Size;

	// U and V are the -1..1 texture coordinate on the current face.
	// Get projected area for this texel
	Ceng::FLOAT32 x0 = U - InvResolution;
	Ceng::FLOAT32 y0 = V - InvResolution;
	Ceng::FLOAT32 x1 = U + InvResolution;
	Ceng::FLOAT32 y1 = V + InvResolution;
	Ceng::FLOAT32 SolidAngle = AreaElement(x0, y0) - AreaElement(x0, y1) - AreaElement(x1, y0) + AreaElement(x1, y1);

	return SolidAngle;
}


const Ceng::CubemapFace::value faceArray[6] = { Ceng::CubemapFace::positive_x,
Ceng::CubemapFace::negative_x, Ceng::CubemapFace::positive_y,
Ceng::CubemapFace::negative_y, Ceng::CubemapFace::positive_z, Ceng::CubemapFace::negative_z };

struct IrradianceThreadCommon
{
	Ceng::UINT32 sourceWidthInt;
	Ceng::UINT32 destWidthInt;
	Ceng::FLOAT32 invDestWidth;
	Ceng::Matrix4* faceMatrix;
	Ceng::UINT32 faceSize;
	Ceng::FLOAT32* solidAngleRayDir;
	Ceng::FLOAT32* sourceMap;
};

class IrradianceMapThread_v1
{
public:

	const IrradianceThreadCommon& common;
	Ceng::UINT32 destFace;
	Ceng::FLOAT32* out_map;

	double& out_duration;

	IrradianceMapThread_v1(const IrradianceThreadCommon& _common, const Ceng::UINT32 _destFace, Ceng::FLOAT32* _out_map,
		double& _out_duration)
		: common(_common), destFace(_destFace), out_map(_out_map),out_duration(_out_duration)
	{

	}

	void operator() ()
	{
		double start = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destWidthInt; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destWidthInt; ++destU)
			{
				// Destination vector (surface normal)
				Ceng::FLOAT32 normal[3];

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, normal);

				Ceng::FLOAT32* dest = &out_map[4 * (destV * common.destWidthInt + destU)];

				dest[0] = 0.0;
				dest[1] = 0.0;
				dest[2] = 0.0;
				dest[3] = 1.0;

				for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
				{
					for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceWidthInt; ++sourceV)
					{
						for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceWidthInt; ++sourceU)
						{
							Ceng::FLOAT32* cache = &common.solidAngleRayDir[4 * (sourceV * common.sourceWidthInt + sourceU)];

							// Get light source direction for +X face and rotate it for the current face
							Ceng::VectorF4 dir;

							dir.x = cache[0];
							dir.y = cache[1];
							dir.z = cache[2];
							dir.w = 1.0f;

							dir = common.faceMatrix[sourceFace] * dir;

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal[0] * dir.x + normal[1] * dir.y + normal[2] * dir.z;

							// Adjust by solid angle

							dot *= cache[3];

							if (dot > 0.0f)
							{
								Ceng::FLOAT32* source = &common.sourceMap[sourceFace * common.faceSize + 4 * (sourceV * common.sourceWidthInt + sourceU)];

								dest[0] += dot * source[0];
								dest[1] += dot * source[1];
								dest[2] += dot * source[2];
							}
						}
					}
				}

			}
		}

		double end = Ceng_HighPrecisionTimer();

		out_duration = end - start;
	}
};

EngineResult::value IrradianceConvolution_v1(const Ceng::UINT32 faceSize, const Ceng::UINT32 destWidthInt, const Ceng::UINT32 sourceWidthInt,
	const Ceng::FLOAT32 invDestWidth, Ceng::Matrix4* faceMatrix, Ceng::FLOAT32* solidAngleRayDir, Ceng::FLOAT32* sourceMap, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);
	//std::vector<Ceng::FLOAT32> destMap(4 * destWidthInt*destWidthInt);

	Ceng::FLOAT32* destMap[6];

	for (int i = 0; i < 6; i++)
	{
		destMap[i] = (Ceng::FLOAT32*)malloc(4 * destWidthInt * destWidthInt * sizeof(Ceng::FLOAT32));
	}

	// Convolve irradiance map from env map

	IrradianceThreadCommon common;

	common.destWidthInt = destWidthInt;
	common.sourceWidthInt = sourceWidthInt;
	common.solidAngleRayDir = solidAngleRayDir;
	common.faceMatrix = faceMatrix;
	common.faceSize = faceSize;
	common.invDestWidth = invDestWidth;
	common.sourceMap = sourceMap;

	double start = Ceng_HighPrecisionTimer();

	double duration[6];

	IrradianceMapThread_v1 tasks[6] =
	{
		{common, 0, destMap[0],duration[0]},
		{common, 1, destMap[1],duration[1]},
		{common, 2, destMap[2],duration[2]},
		{common, 3, destMap[3],duration[3]},
		{common, 4, destMap[4],duration[4]},
		{common, 5, destMap[5],duration[5]},
	};

	std::thread t0(tasks[0]);
	std::thread t1(tasks[1]);
	std::thread t2(tasks[2]);
	std::thread t3(tasks[3]);
	std::thread t4(tasks[4]);
	std::thread t5(tasks[5]);

	t0.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	double end = Ceng_HighPrecisionTimer();

	Ceng::StringUtf8 text;

	for (int i = 0; i < 6; i++)
	{
		text = "dest face ";
		text += i;
		text += ": Took ";
		text += duration[i];
		Ceng::Log::Print(text);
	}

	text = "total convolution time: ";
	text += end - start;
	Ceng::Log::Print(text);

	// Copy results to GPU

	EngineResult::value eresult = EngineResult::ok;

	for (Ceng::UINT32 destFace = 0; destFace < 6; ++destFace)
	{
		Ceng::CRESULT cresult = irradianceMap->SetSubResourceData(faceArray[destFace], 0, Ceng::IMAGE_FORMAT::unorm_a8_b8_g8_r8, &destMap[destFace]);
		if (cresult != Ceng::CE_OK)
		{
			eresult = EngineResult::fail;
			break;
		}
	}

	for (int i = 0; i < 6; i++)
	{
		free(destMap[i]);
	}

	return eresult;
}

const EngineResult::value CEngine::CreateIrradianceMap(Ceng::Cubemap *envMap, Ceng::Cubemap *irradianceMap)
{
	Ceng::Log::Print("Irradiance map generator");

	Ceng::BufferData2D buffer;

	envMap->GetBufferData2D(&buffer);

	Ceng::FLOAT32 invSourceWidth = 1.0f / Ceng::FLOAT32(buffer.width);
	Ceng::UINT32 sourceWidthInt = buffer.width;

	Ceng::StringUtf8 text;

	text = "source width = ";
	text += buffer.width;
	Ceng::Log::Print(text);

	irradianceMap->GetBufferData2D(&buffer);

	Ceng::FLOAT32 invDestWidth = 1.0f / Ceng::FLOAT32(buffer.width);
	Ceng::UINT32 destWidthInt = buffer.width;

	text = "dest width = ";
	text += buffer.width;
	Ceng::Log::Print(text);

	Ceng::FLOAT64 start, end;

	start = Ceng_HighPrecisionTimer();

	// Precalculate solid angles per texel
	
	//std::vector<Ceng::FLOAT32> solidAngleRayDir(4 * sourceWidthInt*sourceWidthInt);

	Ceng::FLOAT32* solidAngleRayDir = (Ceng::FLOAT32*)malloc(4 * sourceWidthInt * sourceWidthInt * sizeof(sizeof(Ceng::FLOAT32)));

	for (Ceng::UINT32 sourceV = 0; sourceV < sourceWidthInt; ++sourceV)
	{
		for (Ceng::UINT32 sourceU = 0; sourceU < sourceWidthInt; ++sourceU)
		{
			Ceng::FLOAT32 *target = &solidAngleRayDir[4 * (sourceV*sourceWidthInt + sourceU)];

			Ceng::FLOAT32 lightDir[3];

			CEngine::RayDir(sourceU, sourceV, faceArray[0], invSourceWidth, lightDir);

			// Dot product between surface normal and light direction
			//Ceng::FLOAT32 dot = normal[0] * lightDir[0] + normal[1] * lightDir[1] + normal[2] * lightDir[2];

			// Store direction vector for +X face
			target[0] = lightDir[0];
			target[1] = lightDir[1];
			target[2] = lightDir[2];

			// Solid angle is same for all faces
			target[3] = TexelCoordSolidAngle(0, Ceng::FLOAT32(sourceU), Ceng::FLOAT32(sourceV), sourceWidthInt);

			/*
			if (dot > 0.0f)
			{
				Ceng::FLOAT32 *source = &sourceMap[4 * (sourceV*sourceWidthInt + sourceU)];

				dest[0] += dot * source[0];
				dest[1] += dot * source[1];
				dest[2] += dot * source[2];
			}
			*/
		}
	}

	end = Ceng_HighPrecisionTimer();

	text = "solid angle precalc, t = ";
	text += end - start;
	Ceng::Log::Print(text);

	// Generate face matrices

	Ceng::Matrix4 faceMatrix[6];

	for (auto &matrix : faceMatrix)
	{
		matrix.Identity();
	}

	// For +X direction is
	// m -tc -sc

	// From +X to -X
	// -m -tc sc
	faceMatrix[1].data[0][0] = -1.0f;
	faceMatrix[1].data[2][2] = -1.0f;

	// From +X to +Y
	// sc m tc
	faceMatrix[2].data[0][0] = 0.0f;
	faceMatrix[2].data[0][2] = -1.0f;
	
	faceMatrix[2].data[1][0] = 1.0f;
	faceMatrix[2].data[1][1] = 0.0f;

	faceMatrix[2].data[2][1] = -1.0f;
	faceMatrix[2].data[2][2] = 0.0f;

	// From +X to -Y
	// sc -m -tc
	faceMatrix[3].data[0][0] = 0.0f;
	faceMatrix[3].data[0][2] = -1.0f;

	faceMatrix[3].data[1][0] = -1.0f;
	faceMatrix[3].data[1][1] = 0.0f;

	faceMatrix[3].data[2][1] = 1.0f;
	faceMatrix[3].data[2][2] = 0.0f;

	// From +X to +Z
	// sc -tc m
	faceMatrix[4].data[0][0] = 0.0f;
	faceMatrix[4].data[0][2] = -1.0f;

	faceMatrix[4].data[2][0] = 1.0f;
	faceMatrix[4].data[2][2] = 0.0f;

	// From +X to -Z
	// -sc -tc -m
	faceMatrix[5].data[0][0] = 0.0f;
	faceMatrix[5].data[0][2] = 1.0f;

	faceMatrix[5].data[2][0] = -1.0f;
	faceMatrix[5].data[2][2] = 0.0f;

	Ceng::UINT32 faceSize = 4 * sourceWidthInt*sourceWidthInt;

	//std::vector<Ceng::FLOAT32> sourceMap(6*faceSize);

	Ceng::FLOAT32* sourceMap = (Ceng::FLOAT32*)malloc(6 * faceSize * sizeof(Ceng::FLOAT32));

	// Copy source maps to RAM

	Ceng::CRESULT cresult;

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		cresult = envMap->GetSubResourceData(faceArray[sourceFace], 0, Ceng::IMAGE_FORMAT::unorm_a8_b8_g8_r8, &sourceMap[sourceFace * faceSize]);
		if (cresult != Ceng::CE_OK)
		{
			free(solidAngleRayDir);
			free(sourceMap);
			return EngineResult::fail;
		}
	}

	end = Ceng_HighPrecisionTimer();

	text = "source copy. t = ";
	text += end - start;
	Ceng::Log::Print(text);

	EngineResult::value eresult = IrradianceConvolution_v1(faceSize,destWidthInt,sourceWidthInt,invDestWidth,faceMatrix,solidAngleRayDir,sourceMap,irradianceMap);

	free(solidAngleRayDir);
	free(sourceMap);

	return eresult;
}

void CEngine::RayDir(const Ceng::UINT32 u, const Ceng::UINT32 v, const Ceng::CubemapFace::value face,
	const Ceng::FLOAT32 invCubeWidth, Ceng::FLOAT32 *out_dir)
{
	Ceng::FLOAT32 s = u * invCubeWidth;
	Ceng::FLOAT32 t = v * invCubeWidth;

	// Adjust to [-1,1] range

	s = 2.0f*s - 1.0f;

	t = 2.0f*t - 1.0f;

	Ceng::FLOAT32 sc, tc, m;

	// Translation from input vector,
	// where m = major axis component of the vector
	//s = sc / abs(m);
	//t = tc / abs(m);

	// Assume input vector is unit length, so
	// dot = sc*sc + tc*tc + m*m = s*s*m*m + t*t*m*m + m*m
	// dot = m*m(1+s*s+t*t) = 1

	m = sqrt(1.0f / (1.0f + s*s + t*t) );

	sc = s*m;
	tc = t*m;

	switch (face)
	{
	case Ceng::CubemapFace::positive_x:
		out_dir[0] = m;
		out_dir[1] = -tc;
		out_dir[2] = -sc;
		break;
	case Ceng::CubemapFace::negative_x:
		out_dir[0] = -m;
		out_dir[1] = -tc;
		out_dir[2] = sc;
		break;
	case Ceng::CubemapFace::positive_y:
		out_dir[0] = sc;
		out_dir[1] = m;
		out_dir[2] = tc;
		break;
	case Ceng::CubemapFace::negative_y:
		out_dir[0] = sc;
		out_dir[1] = -m;
		out_dir[2] = -tc;
		break;
	case Ceng::CubemapFace::positive_z:
		out_dir[0] = sc;
		out_dir[1] = -tc;
		out_dir[2] = m;
		break;
	case Ceng::CubemapFace::negative_z:
		out_dir[0] = -sc;
		out_dir[1] = -tc;
		out_dir[2] = -m;
		break;
	}
}
