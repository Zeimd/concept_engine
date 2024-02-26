/*****************************************************************************
*
* env-map.cpp
*
* By Jari Korkala 25/11/2015
*
*****************************************************************************/

#include <ceng/lib/timerlib.h>

#include <thread>

#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

#include "math-vector.h"
#include "env-map.h"

#include "image-format-string.h"

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

const BasisVec4 cubemapBasis[6] =
{
	// positive_x
	{
		{1.0f,0.0f,0.0f}, // forward
		{0.0f,0.0f,1.0f}, // right
		{0.0f,1.0f,0.0f}, // up
	},

	// negative_x
	{
		{-1.0f,0.0f,0.0f}, // forward
		{0.0f,0.0f,-1.0f}, // right
		{0.0f,0.0f,1.0f},  // up
	},

	// positive_y
	{
		{0.0f,1.0f,0.0f},
		{1.0f,0.0f,0.0f},
		{0.0f,0.0f,1.0f},
	},

	// negative_y
	{
		{0.0f,-1.0f,0.0f},
		{1.0f,0.0f,0.0f},
		{0.0f,0.0f,-1.0f},
	},

	// positive_z
	{
		{0.0f,0.0f,1.0f},
		{-1.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f},
	},

	// negative_z
	{
		{0.0f,0.0f,-1.0f},
		{1.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f},
	},
};

void CEngine::PrecalculateNormals(IrradianceThreadCommon& common, Vec4** out_normals)
{
	Ceng::UINT32 normalCount = 6 * common.destMap->width * common.destMap->width;

	*out_normals = (Vec4*)malloc(normalCount * sizeof(Vec4));

	for (int destFace = 0; destFace < 6; destFace++)
	{
		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec4* normal = &(*out_normals)[6 * destFace + destV * common.destMap->width + destU];

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0f;
				dest->y = 0.0f;
				dest->z = 0.0f;
				dest->w = 1.0f;
			}
		}
	}
}




class IrradianceTask_v4
{
public:

	IrradianceThreadCommon& common;
	TaskData_v3* taskData;
	Ceng::UINT32 first;
	Ceng::UINT32 amount;
	double& out_duration;

	IrradianceTask_v4(IrradianceThreadCommon& _common, TaskData_v3* _taskData, Ceng::UINT32 _first, Ceng::UINT32 _amount, double& _out_duration)
		: common(_common), taskData(_taskData), first(_first), amount(_amount), out_duration(_out_duration)
	{

	}

	void operator() ()
	{
		double start = Ceng_HighPrecisionTimer();

		for (int k = 0; k < amount; k++)
		{
			TaskData_v3* params = &taskData[first + k];

			// Load matrix. The last column isn't needed since we are only using the 3x3 portion of the matrix

			__m128 col0 = _mm_loadu_ps(&common.faceTranspose[params->sourceFace].data[0][0]);
			__m128 col1 = _mm_loadu_ps(&common.faceTranspose[params->sourceFace].data[1][0]);
			__m128 col2 = _mm_loadu_ps(&common.faceTranspose[params->sourceFace].data[2][0]);
			//__m128 col3 = _mm_loadu_ps(&common.faceTranspose[params->sourceFace].data[3][0]);

			for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
			{
				for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
				{
					Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

					// Get light source direction for +X face and rotate it for the current face

					__m128 inputDir = _mm_loadu_ps(&cache->x);

					// We know that our matrix is a 3x3 rotation matrix, and we only need xyz of output
					// vector, so we can skip the fourth dimension

					__m128i allX = _mm_shuffle_epi32(*(__m128i*) & inputDir, 0b0);
					__m128i allY = _mm_shuffle_epi32(*(__m128i*) & inputDir, 0b01010101);
					__m128i allZ = _mm_shuffle_epi32(*(__m128i*) & inputDir, 0b10101010);
					//__m128i wVec = _mm_shuffle_epi32(*(__m128i*) & inputDir, 0b11111111);

					__m128 prod0 = _mm_mul_ps(col0, *(__m128*) & allX);
					__m128 prod1 = _mm_mul_ps(col1, *(__m128*) & allY);
					__m128 prod2 = _mm_mul_ps(col2, *(__m128*) & allZ);
					//__m128 prod3 = _mm_mul_ps(col3, *(__m128*) & wVec);

					__m128 resA = _mm_add_ps(prod0, prod1);
					__m128 rotatedDir = _mm_add_ps(resA, prod2);

					__m128 normal = _mm_loadu_ps(&params->normal->x);

					__m128 dot = _mm_dp_ps(rotatedDir, normal, 0b01111111);

					// dot = [dot,dot,dot,dot]

					__m128 solidAngle = _mm_shuffle_ps(inputDir, inputDir, 0b11111111);

					// solidAngle = [solidAngle,solidAngle,solidAngle,solidAngle]

					__m128 factor = _mm_mul_ps(solidAngle, dot);

					// factor = [factor,factor,factor,factor]

					__m128 allZero = _mm_setzero_ps();

					__m128 mask = _mm_cmpgt_ps(factor, allZero);

					factor = _mm_and_ps(mask, factor);

					// now factor is:
					// zero if dot <= 0
					// dot*solidAngle otherwise

					Vec4* source =
						&common.sourceMap->faceData[params->sourceFace][sourceV * common.sourceMap->width + sourceU];

					__m128 sourceVec = _mm_loadu_ps(&source->x);

					sourceVec = _mm_mul_ps(factor, sourceVec);

					__m128 output = _mm_loadu_ps(&params->output.x);

					output = _mm_add_ps(output, sourceVec);

					_mm_storeu_ps(&params->output.x, output);
				}
			}
		}

		double end = Ceng_HighPrecisionTimer();

		out_duration = end - start;
	}
};

class IrradianceTask_v3b
{
public:

	IrradianceThreadCommon& common;
	TaskData_v3* taskData;
	Ceng::UINT32 first;
	Ceng::UINT32 amount;
	double& out_duration;

	IrradianceTask_v3b(IrradianceThreadCommon& _common, TaskData_v3* _taskData, Ceng::UINT32 _first, Ceng::UINT32 _amount, double& _out_duration)
		: common(_common), taskData(_taskData), first(_first), amount(_amount), out_duration(_out_duration)
	{

	}

	void operator() ()
	{
		double start = Ceng_HighPrecisionTimer();

		for (int k = 0; k < amount; k++)
		{
			TaskData_v3* params = &taskData[first + k];

			for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
			{
				for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
				{
					Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

					// Get light source direction for +X face and rotate it for the current face
					Ceng::VectorF4 dir;

					dir.x = cache->x;
					dir.y = cache->y;
					dir.z = cache->z;
					dir.w = 1.0f;

					dir = common.faceTranspose[params->sourceFace].TransposeMul(dir);

					// Dot product between surface normal and light direction
					Ceng::FLOAT32 dot = params->normal->x * dir.x + params->normal->y * dir.y + params->normal->z * dir.z;

					// Adjust by solid angle

					dot *= cache->w;

					if (dot > 0.0f)
					{
						Vec4* source = &common.sourceMap->faceData[params->sourceFace][sourceV * common.sourceMap->width + sourceU];

						params->output.x += dot * source->x;
						params->output.y += dot * source->y;
						params->output.z += dot * source->z;
					}
				}
			}
		}

		double end = Ceng_HighPrecisionTimer();

		out_duration = end - start;
	}
};

class IrradianceTask_v3
{
public:

	IrradianceThreadCommon& common;
	TaskData_v3* taskData;
	Ceng::UINT32 first;
	Ceng::UINT32 amount;
	double& out_duration;

	IrradianceTask_v3(IrradianceThreadCommon& _common, TaskData_v3* _taskData, Ceng::UINT32 _first, Ceng::UINT32 _amount, double& _out_duration)
		: common(_common), taskData(_taskData), first(_first),amount(_amount), out_duration(_out_duration)
	{

	}

	void operator() ()
	{
		double start = Ceng_HighPrecisionTimer();

		for (int k = 0; k < amount; k++)
		{
			TaskData_v3* params = &taskData[first + k];

			for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
			{
				for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
				{
					Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

					// Get light source direction for +X face and rotate it for the current face
					Ceng::VectorF4 dir;

					dir.x = cache->x;
					dir.y = cache->y;
					dir.z = cache->z;
					dir.w = 1.0f;

					dir = common.faceMatrix[params->sourceFace] * dir;

					// Dot product between surface normal and light direction
					Ceng::FLOAT32 dot = params->normal->x * dir.x + params->normal->y * dir.y + params->normal->z * dir.z;

					// Adjust by solid angle

					dot *= cache->w;

					if (dot > 0.0f)
					{
						Vec4* source = &common.sourceMap->faceData[params->sourceFace][sourceV * common.sourceMap->width + sourceU];

						params->output.x += dot * source->x;
						params->output.y += dot * source->y;
						params->output.z += dot * source->z;
					}
				}
			}
		}

		double end = Ceng_HighPrecisionTimer();

		out_duration = end - start;
	}
};

// Compared to v2, eliminates the need to re-create threads between source face loops
template<class IRRADIANCE_TASK>
EngineResult::value IrradianceConvolution_v3_Base(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	// Convolve irradiance map from env map

	double total_start = Ceng_HighPrecisionTimer();

	Ceng::StringUtf8 text;

	Ceng::UINT32 taskCount = 6 * 6 * common.destMap->width * common.destMap->width;

	TaskData_v3* taskData = (TaskData_v3*)malloc(taskCount * sizeof(TaskData_v3));

	Ceng::UINT32 normalCount = 6 * common.destMap->width * common.destMap->width;

	Vec4* normals = (Vec4*)malloc(normalCount * sizeof(Vec4));

	int taskIndex = 0;

	// Precalculate normals

	for (int destFace = 0; destFace < 6; destFace++)
	{
		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec4* normal = &normals[6*destFace + destV * common.destMap->width + destU];

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0f;
				dest->y = 0.0f;
				dest->z = 0.0f;
				dest->w = 1.0f;

				for (int sourceFace = 0; sourceFace < 6; sourceFace++)
				{
					taskData[taskIndex].dest = dest;
					taskData[taskIndex].normal = normal;
					taskData[taskIndex].sourceFace = sourceFace;

					taskData[taskIndex].output.x = 0.0f;
					taskData[taskIndex].output.y = 0.0f;
					taskData[taskIndex].output.z = 0.0f;
					taskData[taskIndex].output.w = 0.0f;

					++taskIndex;
				}
			}
		}
	}

	const Ceng::UINT32 threadCount = 6;

	Ceng::UINT32 taskPerThread = taskCount / threadCount;
	Ceng::UINT32 taskRemainder = taskCount % threadCount;

	std::vector<std::thread> threads;

	Ceng::UINT32 firstTask = 0;

	double* durations = (double*)malloc(threadCount * sizeof(double));

	for (int i = 0; i < threadCount-1; i++)
	{
		threads.emplace_back(IRRADIANCE_TASK(common, taskData, firstTask, taskPerThread, durations[i]));

		firstTask += taskPerThread;
	}

	threads.emplace_back(IRRADIANCE_TASK(common, taskData, firstTask, taskPerThread + taskRemainder, durations[threadCount-1]));

	for (auto& x : threads)
	{
		x.join();
	}

	// Accumulate results

	for (int k = 0; k < taskCount; k++)
	{
		TaskData_v3* task = &taskData[k];

		task->dest->x += task->output.x;
		task->dest->y += task->output.y;
		task->dest->z += task->output.z;
	}

	double total_end = Ceng_HighPrecisionTimer();

	for (int k = 0; k < threadCount; k++)
	{
		text = "thread ";
		text += k;
		text += ": Took ";
		text += durations[k];
		Ceng::Log::Print(text);
	}

	text = "total convolution time: ";
	text += total_end - total_start;
	Ceng::Log::Print(text);

	free(taskData);
	free(normals);
	free(durations);

	return EngineResult::ok;
}

auto IrradianceConvolution_v3 = IrradianceConvolution_v3_Base< IrradianceTask_v3>;
auto IrradianceConvolution_v3b = IrradianceConvolution_v3_Base< IrradianceTask_v3b>;
auto IrradianceConvolution_v4 = IrradianceConvolution_v3_Base< IrradianceTask_v4>;

class IrradianceMapTask_v2
{
public:

	IrradianceThreadCommon& common;

	Vec4* dest;
	Ceng::UINT32 sourceFace;
	Vec3* normal;

	IrradianceMapTask_v2(IrradianceThreadCommon& _common, Ceng::UINT32 _sourceFace, Vec3* _normal, Vec4* _dest)
		: common(_common), sourceFace(_sourceFace), normal(_normal), dest(_dest)
	{

	}

	void operator() ()
	{
		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

				// Get light source direction for +X face and rotate it for the current face
				Ceng::VectorF4 dir;

				dir.x = cache->x;
				dir.y = cache->y;
				dir.z = cache->z;
				dir.w = 1.0f;

				dir = common.faceMatrix[sourceFace] * dir;

				// Dot product between surface normal and light direction
				Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

				// Adjust by solid angle

				dot *= cache->w;

				if (dot > 0.0f)
				{
					Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

					dest->x += dot * source->x;
					dest->y += dot * source->y;
					dest->z += dot * source->z;
				}
			}
		}
	}

};

// Use multithreading at source loop level
EngineResult::value IrradianceConvolution_v2(IrradianceThreadCommon &common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	// Convolve irradiance map from env map

	double total_start = Ceng_HighPrecisionTimer();

	Ceng::StringUtf8 text;

	for (int destFace = 0; destFace < 6; destFace++)
	{
		double face_start = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec3 normal;

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, &normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0;
				dest->y = 0.0;
				dest->z = 0.0;
				dest->w = 1.0;

				Vec4 threadDest[6];
				for (int k = 0; k < 6; k++)
				{
					threadDest[k].x = 0.0f;
					threadDest[k].y = 0.0f;
					threadDest[k].z = 0.0f;
					threadDest[k].w = 1.0f;
				}

				IrradianceMapTask_v2 tasks[6] =
				{
					{common, 0, &normal, &threadDest[0]},
					{common, 1, &normal, &threadDest[1]},
					{common, 2, &normal, &threadDest[2]},
					{common, 3, &normal, &threadDest[3]},
					{common, 4, &normal, &threadDest[4]},
					{common, 5, &normal, &threadDest[5]},
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

				for (int i = 0; i < 6; i++)
				{
					dest->x += threadDest[i].x;
					dest->y += threadDest[i].y;
					dest->z += threadDest[i].z;					
				}
			}
		}

		double face_end = Ceng_HighPrecisionTimer();

		text = "dest face ";
		text += destFace;
		text += ": Took ";
		text += face_end - face_start;
		Ceng::Log::Print(text);
	}

	double total_end = Ceng_HighPrecisionTimer();

	text = "total convolution time: ";
	text += total_end - total_start;
	Ceng::Log::Print(text);

	// Copy results to GPU

	return EngineResult::ok;
}



class IrradianceMapTask_v1
{
public:

	const IrradianceThreadCommon& common;
	Ceng::UINT32 destFace;

	double& out_duration;

	IrradianceMapTask_v1(const IrradianceThreadCommon& _common, const Ceng::UINT32 _destFace,
		double& _out_duration)
		: common(_common), destFace(_destFace), out_duration(_out_duration)
	{

	}

	void operator() ()
	{
		double start = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec3 normal;

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, &normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0;
				dest->y = 0.0;
				dest->z = 0.0;
				dest->w = 1.0;

				for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
				{
					for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
					{
						for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
						{
							Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

							// Get light source direction for +X face and rotate it for the current face
							Ceng::VectorF4 dir;

							dir.x = cache->x;
							dir.y = cache->y;
							dir.z = cache->z;
							dir.w = 1.0f;

							dir = common.faceMatrix[sourceFace] * dir;

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal.x * dir.x + normal.y * dir.y + normal.z * dir.z;

							// Adjust by solid angle

							dot *= cache->w;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
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

// Use multithreading at dest face loop level
EngineResult::value IrradianceConvolution_v1(IrradianceThreadCommon &common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	// Convolve irradiance map from env map

	double start = Ceng_HighPrecisionTimer();

	double duration[6];

	IrradianceMapTask_v1 tasks[6] =
	{
		{common, 0,duration[0]},
		{common, 1,duration[1]},
		{common, 2,duration[2]},
		{common, 3,duration[3]},
		{common, 4,duration[4]},
		{common, 5,duration[5]},
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

	return EngineResult::ok;
}

class IrradianceMapTask_v0e_e
{
public:

	const IrradianceThreadCommon& common;
	Ceng::UINT32 sourceFace;
	Vec4* normals;

	CubemapData& out_data;

	double& out_duration;

	IrradianceMapTask_v0e_e(const IrradianceThreadCommon& _common, const Ceng::UINT32 _sourceFace, Vec4* _normals,
		CubemapData& _out_data, double& _out_duration)
		: common(_common), sourceFace(_sourceFace), normals(_normals),out_data(_out_data),out_duration(_out_duration)
	{

	}

	void operator() ()
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Ceng::UINT32 fetchU = sourceU;
				if (fetchU > common.quadrantWidth)
				{
					fetchU = 2 * common.quadrantWidth - fetchU;
				}

				Ceng::UINT32 fetchV = sourceV;

				if (fetchV > common.quadrantWidth)
				{
					fetchV = 2 * common.quadrantWidth - fetchV;
				}

				Ceng::FLOAT32 solidAngle = common.solidAngleOnly[fetchV * common.quadrantWidth + fetchU];

				Vec4 dir;

				Ceng::INT32 uDelta = sourceU - (common.sourceMap->width >> 1);
				Ceng::INT32 vDelta = sourceV - (common.sourceMap->width >> 1);

				dir.x = cubemapBasis[sourceFace].forward.x * (common.sourceMap->width >> 1);
				dir.y = cubemapBasis[sourceFace].forward.y * (common.sourceMap->width >> 1);
				dir.z = cubemapBasis[sourceFace].forward.z * (common.sourceMap->width >> 1);

				dir.x += cubemapBasis[sourceFace].right.x * uDelta;
				dir.y += cubemapBasis[sourceFace].right.y * uDelta;
				dir.z += cubemapBasis[sourceFace].right.z * uDelta;

				dir.x += cubemapBasis[sourceFace].up.x * vDelta;
				dir.y += cubemapBasis[sourceFace].up.y * vDelta;
				dir.z += cubemapBasis[sourceFace].up.z * vDelta;

				Ceng::FLOAT32 normDiv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

				dir.x *= normDiv;
				dir.y *= normDiv;
				dir.z *= normDiv;

				for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
				{
					for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
					{
						for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
						{
							// Destination vector (surface normal)
							Vec4* normal = &normals[6 * destFace + destV * common.destMap->width + destU];

							Vec4* dest = &out_data.faceData[destFace][destV * common.destMap->width + destU];

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

							dot *= solidAngle;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		out_duration = faceEnd - faceStart;
	}
};


// v0e_e with multithreading
EngineResult::value IrradianceConvolution_v0e_e_m(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	Ceng::StringUtf8 text;

	double start, end;

	Vec4* normals = nullptr;

	// Precalculate normals and clear ooutput

	start = Ceng_HighPrecisionTimer();

	PrecalculateNormals(common, &normals);

	end = Ceng_HighPrecisionTimer();

	text = "precalculate destination normals. Took ";
	text += end - start;
	Ceng::Log::Print(text);

	// Convolve irradiance map from env map

	Ceng::UINT32 quadrantWidth = common.sourceMap->width >> 1;

	start = Ceng_HighPrecisionTimer();

	CubemapData tempCubes[5] =
	{
		CubemapData(common.destMap->width),
		CubemapData(common.destMap->width),
		CubemapData(common.destMap->width),
		CubemapData(common.destMap->width),
		CubemapData(common.destMap->width),
	};

	double duration[6];

	std::thread t0(IrradianceMapTask_v0e_e(common,0, normals, *common.destMap, duration[0]));
	std::thread t1(IrradianceMapTask_v0e_e(common,1, normals, tempCubes[0], duration[1]));
	std::thread t2(IrradianceMapTask_v0e_e(common,2, normals, tempCubes[1], duration[2]));
	std::thread t3(IrradianceMapTask_v0e_e(common,3, normals, tempCubes[2], duration[3]));
	std::thread t4(IrradianceMapTask_v0e_e(common,4, normals, tempCubes[3], duration[4]));
	std::thread t5(IrradianceMapTask_v0e_e(common,5, normals, tempCubes[4], duration[5]));

	t0.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	for (int i = 0; i < 5; i++)
	{
		for (int face = 0; face < 6; face++)
		{
			for (int k = 0; k < common.destMap->faceSize; k++)
			{
				common.destMap->faceData[face][k].x += tempCubes[i].faceData[face][k].x;
				common.destMap->faceData[face][k].y += tempCubes[i].faceData[face][k].y;
				common.destMap->faceData[face][k].z += tempCubes[i].faceData[face][k].z;
			}
		}
	}

	end = Ceng_HighPrecisionTimer();

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

	free(normals);

	return EngineResult::ok;
}


// Like v0e_c, but uses quadrant version of solid angle lookup
EngineResult::value IrradianceConvolution_v0e_e(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	Ceng::StringUtf8 text;

	double start, end;

	Vec4* normals = nullptr;

	// Precalculate normals and clear ooutput

	start = Ceng_HighPrecisionTimer();

	PrecalculateNormals(common, &normals);

	end = Ceng_HighPrecisionTimer();

	text = "precalculate destination normals. Took ";
	text += end - start;
	Ceng::Log::Print(text);

	double duration[6];

	// Convolve irradiance map from env map

	Ceng::UINT32 quadrantWidth = common.sourceMap->width >> 1;

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Ceng::UINT32 fetchU = sourceU;
				if (fetchU > quadrantWidth)
				{
					fetchU = 2 * quadrantWidth - fetchU;
				}

				Ceng::UINT32 fetchV = sourceV;

				if (fetchV > quadrantWidth)
				{
					fetchV = 2 * quadrantWidth - fetchV;
				}

				Ceng::FLOAT32 solidAngle = common.solidAngleOnly[fetchV * quadrantWidth + fetchU];
		
				Vec4 dir;

				Ceng::INT32 uDelta = sourceU - (common.sourceMap->width >> 1);
				Ceng::INT32 vDelta = sourceV - (common.sourceMap->width >> 1);

				dir.x = cubemapBasis[sourceFace].forward.x * (common.sourceMap->width >> 1);
				dir.y = cubemapBasis[sourceFace].forward.y * (common.sourceMap->width >> 1);
				dir.z = cubemapBasis[sourceFace].forward.z * (common.sourceMap->width >> 1);

				dir.x += cubemapBasis[sourceFace].right.x * uDelta;
				dir.y += cubemapBasis[sourceFace].right.y * uDelta;
				dir.z += cubemapBasis[sourceFace].right.z * uDelta;

				dir.x += cubemapBasis[sourceFace].up.x * vDelta;
				dir.y += cubemapBasis[sourceFace].up.y * vDelta;
				dir.z += cubemapBasis[sourceFace].up.z * vDelta;

				Ceng::FLOAT32 normDiv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

				dir.x *= normDiv;
				dir.y *= normDiv;
				dir.z *= normDiv;

				for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
				{
					for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
					{
						for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
						{
							// Destination vector (surface normal)
							Vec4* normal = &normals[6 * destFace + destV * common.destMap->width + destU];

							Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

							dot *= solidAngle;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[sourceFace] = faceEnd - faceStart;
	}

	end = Ceng_HighPrecisionTimer();


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

	free(normals);

	return EngineResult::ok;
}


// Like v0e_b, but calculates solid angle on the fly
EngineResult::value IrradianceConvolution_v0e_d(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	Ceng::StringUtf8 text;

	double start, end;

	Vec4* normals = nullptr;

	// Precalculate normals and clear ooutput

	start = Ceng_HighPrecisionTimer();

	PrecalculateNormals(common, &normals);

	end = Ceng_HighPrecisionTimer();

	text = "precalculate destination normals. Took ";
	text += end - start;
	Ceng::Log::Print(text);

	double duration[6];

	// Convolve irradiance map from env map

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Ceng::FLOAT32 solidAngle = TexelCoordSolidAngle(0, Ceng::FLOAT32(sourceU), Ceng::FLOAT32(sourceV), common.sourceMap->width);

				Vec4 dir;

				Ceng::INT32 uDelta = sourceU - (common.sourceMap->width >> 1);
				Ceng::INT32 vDelta = sourceV - (common.sourceMap->width >> 1);

				dir.x = cubemapBasis[sourceFace].forward.x * (common.sourceMap->width >> 1);
				dir.y = cubemapBasis[sourceFace].forward.y * (common.sourceMap->width >> 1);
				dir.z = cubemapBasis[sourceFace].forward.z * (common.sourceMap->width >> 1);

				dir.x += cubemapBasis[sourceFace].right.x * uDelta;
				dir.y += cubemapBasis[sourceFace].right.y * uDelta;
				dir.z += cubemapBasis[sourceFace].right.z * uDelta;

				dir.x += cubemapBasis[sourceFace].up.x * vDelta;
				dir.y += cubemapBasis[sourceFace].up.y * vDelta;
				dir.z += cubemapBasis[sourceFace].up.z * vDelta;

				Ceng::FLOAT32 normDiv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

				dir.x *= normDiv;
				dir.y *= normDiv;
				dir.z *= normDiv;

				for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
				{
					for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
					{
						for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
						{
							// Destination vector (surface normal)
							Vec4* normal = &normals[6 * destFace + destV * common.destMap->width + destU];

							Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

							dot *= solidAngle;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[sourceFace] = faceEnd - faceStart;
	}

	end = Ceng_HighPrecisionTimer();


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

	free(normals);

	return EngineResult::ok;
}


// Like v0e_b, but uses lightweight lookup-table with solid angle only
EngineResult::value IrradianceConvolution_v0e_c(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	Ceng::StringUtf8 text;

	double start, end;

	Vec4* normals = nullptr;

	// Precalculate normals and clear ooutput

	start = Ceng_HighPrecisionTimer();

	PrecalculateNormals(common, &normals);

	end = Ceng_HighPrecisionTimer();

	text = "precalculate destination normals. Took ";
	text += end - start;
	Ceng::Log::Print(text);

	double duration[6];

	// Convolve irradiance map from env map

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Ceng::FLOAT32 solidAngle = common.solidAngleOnly[sourceV * common.sourceMap->width + sourceU];

				Vec4 dir;

				Ceng::INT32 uDelta = sourceU - (common.sourceMap->width >> 1);
				Ceng::INT32 vDelta = sourceV - (common.sourceMap->width >> 1);

				dir.x = cubemapBasis[sourceFace].forward.x * (common.sourceMap->width >> 1);
				dir.y = cubemapBasis[sourceFace].forward.y * (common.sourceMap->width >> 1);
				dir.z = cubemapBasis[sourceFace].forward.z * (common.sourceMap->width >> 1);

				dir.x += cubemapBasis[sourceFace].right.x * uDelta;
				dir.y += cubemapBasis[sourceFace].right.y * uDelta;
				dir.z += cubemapBasis[sourceFace].right.z * uDelta;

				dir.x += cubemapBasis[sourceFace].up.x * vDelta;
				dir.y += cubemapBasis[sourceFace].up.y * vDelta;
				dir.z += cubemapBasis[sourceFace].up.z * vDelta;

				Ceng::FLOAT32 normDiv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

				dir.x *= normDiv;
				dir.y *= normDiv;
				dir.z *= normDiv;

				for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
				{
					for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
					{
						for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
						{
							// Destination vector (surface normal)
							Vec4* normal = &normals[6 * destFace + destV * common.destMap->width + destU];

							Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

							dot *= solidAngle;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[sourceFace] = faceEnd - faceStart;
	}

	end = Ceng_HighPrecisionTimer();


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

	free(normals);

	return EngineResult::ok;
}


// Like v0e, but light directions calculated from basis vectors instead of lookup + rotation
EngineResult::value IrradianceConvolution_v0e_b(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	Ceng::StringUtf8 text;

	double start, end;

	Vec4* normals = nullptr;

	// Precalculate normals and clear ooutput

	start = Ceng_HighPrecisionTimer();

	PrecalculateNormals(common, &normals);

	end = Ceng_HighPrecisionTimer();

	text = "precalculate destination normals. Took ";
	text += end - start;
	Ceng::Log::Print(text);

	double duration[6];

	// Convolve irradiance map from env map

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

				Vec4 dir;

				Ceng::INT32 uDelta = sourceU - (common.sourceMap->width >> 1);
				Ceng::INT32 vDelta = sourceV - (common.sourceMap->width >> 1);

				dir.x = cubemapBasis[sourceFace].forward.x * (common.sourceMap->width >> 1);
				dir.y = cubemapBasis[sourceFace].forward.y * (common.sourceMap->width >> 1);
				dir.z = cubemapBasis[sourceFace].forward.z * (common.sourceMap->width >> 1);

				dir.x += cubemapBasis[sourceFace].right.x * uDelta;
				dir.y += cubemapBasis[sourceFace].right.y * uDelta;
				dir.z += cubemapBasis[sourceFace].right.z * uDelta;

				dir.x += cubemapBasis[sourceFace].up.x * vDelta;
				dir.y += cubemapBasis[sourceFace].up.y * vDelta;
				dir.z += cubemapBasis[sourceFace].up.z * vDelta;

				Ceng::FLOAT32 normDiv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

				dir.x *= normDiv;
				dir.y *= normDiv;
				dir.z *= normDiv;				

				for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
				{
					for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
					{
						for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
						{
							// Destination vector (surface normal)
							Vec4* normal = &normals[6 * destFace + destV * common.destMap->width + destU];

							Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

							// Adjust by solid angle

							dot *= cache->w;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[sourceFace] = faceEnd - faceStart;
	}

	end = Ceng_HighPrecisionTimer();


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

	free(normals);

	return EngineResult::ok;
}


// Flip inner and outer loops to reduce costly operations
EngineResult::value IrradianceConvolution_v0e(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	Ceng::StringUtf8 text;

	double start, end;

	Vec4* normals = nullptr;

	// Precalculate normals and clear ooutput

	start = Ceng_HighPrecisionTimer();

	PrecalculateNormals(common, &normals);

	end = Ceng_HighPrecisionTimer();

	text = "precalculate destination normals. Took ";
	text += end - start;
	Ceng::Log::Print(text);

	double duration[6];

	// Convolve irradiance map from env map

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
		{
			for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
			{
				Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

				// Get light source direction for +X face and rotate it for the current face
				Ceng::VectorF4 dir;

				dir.x = cache->x;
				dir.y = cache->y;
				dir.z = cache->z;
				dir.w = 1.0f;

				dir = common.faceMatrix[sourceFace] * dir;

				for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
				{
					for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
					{
						for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
						{
							// Destination vector (surface normal)
							Vec4* normal = &normals[6*destFace + destV*common.destMap->width + destU];

							Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal->x * dir.x + normal->y * dir.y + normal->z * dir.z;

							// Adjust by solid angle

							dot *= cache->w;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[sourceFace] = faceEnd - faceStart;
	}

	end = Ceng_HighPrecisionTimer();

	
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

	free(normals);

	return EngineResult::ok;
}


// Test quadrant solid angle lookup table
EngineResult::value IrradianceConvolution_v0d(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	double duration[6];

	// Convolve irradiance map from env map

	double start = Ceng_HighPrecisionTimer();

	Ceng::UINT32 quadrantWidth = common.sourceMap->width >> 1;

	for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec3 normal;

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, &normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0;
				dest->y = 0.0;
				dest->z = 0.0;
				dest->w = 1.0;

				for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
				{
					for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
					{
						for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
						{
							Ceng::UINT32 fetchU = sourceU;
							bool flipU = false;
							if (fetchU > quadrantWidth)
							{
								flipU = true;
								fetchU = 2*quadrantWidth - fetchU;
							}

							Ceng::UINT32 fetchV = sourceV;
							bool flipV = false;

							if (fetchV > quadrantWidth)
							{
								flipV = true;
								fetchV = 2 * quadrantWidth - fetchV;
							}
							
							Vec4* cache = &common.solidAngleRayDir[fetchV *quadrantWidth + fetchU];

							// Get light source direction for +X face and rotate it for the current face
							Ceng::VectorF4 dir;

							dir.x = cache->x;
							dir.y = cache->y;
							dir.z = cache->z;
							dir.w = 1.0f;

							if (flipU)
							{
								dir.y *= -1;
							}

							if (flipV)
							{
								dir.z *= -1;
							}

							dir = common.faceMatrix[sourceFace] * dir;

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal.x * dir.x + normal.y * dir.y + normal.z * dir.z;

							// Adjust by solid angle

							dot *= cache->w;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[destFace] = faceEnd - faceStart;
	}

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

	return EngineResult::ok;
}


// Calculate source with RayDir() instead of using lookup table and rotation
EngineResult::value IrradianceConvolution_v0c(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	double duration[6];

	// Convolve irradiance map from env map

	double start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec3 normal;

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, &normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0;
				dest->y = 0.0;
				dest->z = 0.0;
				dest->w = 1.0;

				for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
				{
					for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
					{
						for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
						{
							Ceng::FLOAT32 solidAngle = common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU].w;

							Vec3 dir;

							CEngine::RayDir(sourceU, sourceV, faceArray[sourceFace], common.invSourceWidth, &dir);

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal.x * dir.x + normal.y * dir.y + normal.z * dir.z;

							dot *= solidAngle;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[destFace] = faceEnd - faceStart;
	}

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

	return EngineResult::ok;
}

// Calculate source direction from basis vectors instead of using lookup table and rotation
EngineResult::value IrradianceConvolution_v0b(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	double duration[6];

	// Convolve irradiance map from env map

	double start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec3 normal;

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, &normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0;
				dest->y = 0.0;
				dest->z = 0.0;
				dest->w = 1.0;

				for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
				{
					for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
					{
						for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
						{
							Ceng::FLOAT32 solidAngle = common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU].w;

							Vec4 dir;

							Ceng::INT32 uDelta = sourceU - (common.sourceMap->width >> 1);
							Ceng::INT32 vDelta = sourceV - (common.sourceMap->width >> 1);

							dir.x = cubemapBasis[sourceFace].forward.x * (common.sourceMap->width >> 1);
							dir.y = cubemapBasis[sourceFace].forward.y * (common.sourceMap->width >> 1);
							dir.z = cubemapBasis[sourceFace].forward.z * (common.sourceMap->width >> 1);

							dir.x += cubemapBasis[sourceFace].right.x * uDelta;
							dir.y += cubemapBasis[sourceFace].right.y * uDelta;
							dir.z += cubemapBasis[sourceFace].right.z * uDelta;

							dir.x += cubemapBasis[sourceFace].up.x * vDelta;
							dir.y += cubemapBasis[sourceFace].up.y * vDelta;
							dir.z += cubemapBasis[sourceFace].up.z * vDelta;

							Ceng::FLOAT32 normDiv = 1.0f / sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

							dir.x *= normDiv;
							dir.y *= normDiv;
							dir.z *= normDiv;

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal.x * dir.x + normal.y * dir.y + normal.z * dir.z;

							dot *= solidAngle;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[destFace] = faceEnd - faceStart;
	}

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

	return EngineResult::ok;
}


EngineResult::value IrradianceConvolution_v0(IrradianceThreadCommon& common, Ceng::Cubemap* irradianceMap)
{
	Ceng::Log::Print(__func__);

	double duration[6];

	// Convolve irradiance map from env map

	double start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 destFace = 0; destFace < 6; destFace++)
	{
		double faceStart = Ceng_HighPrecisionTimer();

		for (Ceng::UINT32 destV = 0; destV < common.destMap->width; ++destV)
		{
			for (Ceng::UINT32 destU = 0; destU < common.destMap->width; ++destU)
			{
				// Destination vector (surface normal)
				Vec3 normal;

				CEngine::RayDir(destU, destV, faceArray[destFace], common.invDestWidth, &normal);

				Vec4* dest = &common.destMap->faceData[destFace][destV * common.destMap->width + destU];

				dest->x = 0.0;
				dest->y = 0.0;
				dest->z = 0.0;
				dest->w = 1.0;

				for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
				{
					for (Ceng::UINT32 sourceV = 0; sourceV < common.sourceMap->width; ++sourceV)
					{
						for (Ceng::UINT32 sourceU = 0; sourceU < common.sourceMap->width; ++sourceU)
						{
							Vec4* cache = &common.solidAngleRayDir[sourceV * common.sourceMap->width + sourceU];

							// Get light source direction for +X face and rotate it for the current face
							Ceng::VectorF4 dir;

							dir.x = cache->x;
							dir.y = cache->y;
							dir.z = cache->z;
							dir.w = 1.0f;

							dir = common.faceMatrix[sourceFace] * dir;

							// Dot product between surface normal and light direction
							Ceng::FLOAT32 dot = normal.x * dir.x + normal.y * dir.y + normal.z * dir.z;

							// Adjust by solid angle

							dot *= cache->w;

							if (dot > 0.0f)
							{
								Vec4* source = &common.sourceMap->faceData[sourceFace][sourceV * common.sourceMap->width + sourceU];

								dest->x += dot * source->x;
								dest->y += dot * source->y;
								dest->z += dot * source->z;
							}
						}
					}
				}

			}
		}

		double faceEnd = Ceng_HighPrecisionTimer();

		duration[destFace] = faceEnd - faceStart;
	}

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
	
	return EngineResult::ok;
}


const EngineResult::value CEngine::CreateIrradianceMap(Ceng::Cubemap *envMap, Ceng::Cubemap *irradianceMap)
{
	Ceng::Log::Print("Irradiance map generator");

	Ceng::StringUtf8 text;

	Ceng::BufferData2D envmapBuffer;

	envMap->GetBufferData2D(&envmapBuffer);

	text = "envmap format = ";
	text += ImageFormatToString(Ceng::IMAGE_FORMAT::value(envmapBuffer.format));
	Ceng::Log::Print(text);

	Ceng::FLOAT32 invSourceWidth = 1.0f / Ceng::FLOAT32(envmapBuffer.width);
	Ceng::UINT32 sourceWidthInt = envmapBuffer.width;

	text = "source width = ";
	text += envmapBuffer.width;
	Ceng::Log::Print(text);

	Ceng::BufferData2D irradiancemapBuffer;

	irradianceMap->GetBufferData2D(&irradiancemapBuffer);

	text = "irradiance map format = ";
	text += ImageFormatToString(Ceng::IMAGE_FORMAT::value(irradiancemapBuffer.format));
	Ceng::Log::Print(text);

	Ceng::FLOAT32 invDestWidth = 1.0f / Ceng::FLOAT32(irradiancemapBuffer.width);
	Ceng::UINT32 destWidthInt = irradiancemapBuffer.width;

	text = "dest width = ";
	text += irradiancemapBuffer.width;
	Ceng::Log::Print(text);

	Ceng::FLOAT64 start, end;

	start = Ceng_HighPrecisionTimer();

	// Precalculate solid angles per texel
	
	Ceng::FLOAT32* solidAngleOnly = (Ceng::FLOAT32*)malloc(sourceWidthInt * sourceWidthInt * sizeof(Ceng::FLOAT32));

	Vec4* solidAngleRayDir = (Vec4*)malloc(sourceWidthInt * sourceWidthInt * sizeof(Vec4));

	for (Ceng::UINT32 sourceV = 0; sourceV < sourceWidthInt; ++sourceV)
	{
		for (Ceng::UINT32 sourceU = 0; sourceU < sourceWidthInt; ++sourceU)
		{
			Vec4 *target = &solidAngleRayDir[sourceV*sourceWidthInt + sourceU];

			Vec3 lightDir;

			CEngine::RayDir(sourceU, sourceV, faceArray[0], invSourceWidth, &lightDir);

			// Store direction vector for +X face
			target->x = lightDir.x;
			target->y = lightDir.y;
			target->z = lightDir.z;

			// Solid angle is same for all faces
			target->w = TexelCoordSolidAngle(0, Ceng::FLOAT32(sourceU), Ceng::FLOAT32(sourceV), sourceWidthInt);

			solidAngleOnly[sourceV * sourceWidthInt + sourceU] = target->w;
		}
	}

	Ceng::UINT32 quadrantWidth = sourceWidthInt >> 1;

	Vec4* solidAngleRayDirQuadrant = (Vec4*)malloc(quadrantWidth * quadrantWidth * sizeof(Vec4));

	Ceng::FLOAT32* solidAngleOnlyQuadrant = (Ceng::FLOAT32*)malloc(quadrantWidth * quadrantWidth * sizeof(Ceng::FLOAT32));

	for (Ceng::UINT32 sourceV = 0; sourceV < quadrantWidth; ++sourceV)
	{
		for (Ceng::UINT32 sourceU = 0; sourceU < quadrantWidth; ++sourceU)
		{
			Vec4* target = &solidAngleRayDirQuadrant[sourceV * quadrantWidth + sourceU];

			Vec3 lightDir;

			CEngine::RayDir(sourceU, sourceV, faceArray[0], invSourceWidth, &lightDir);

			// Store direction vector for +X face
			target->x = lightDir.x;
			target->y = lightDir.y;
			target->z = lightDir.z;

			// Solid angle is same for all faces
			target->w = TexelCoordSolidAngle(0, Ceng::FLOAT32(sourceU), Ceng::FLOAT32(sourceV), sourceWidthInt);

			solidAngleOnlyQuadrant[sourceV * quadrantWidth + sourceU] = target->w;
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

	Ceng::Matrix4 faceTranspose[6];

	for (int i = 0; i < 6; i++)
	{
		faceTranspose[i] = faceMatrix[i].Transpose();
	}

	CubemapData sourceMap(sourceWidthInt);

	// Copy source maps to RAM

	Ceng::CRESULT cresult;

	start = Ceng_HighPrecisionTimer();

	for (Ceng::UINT32 sourceFace = 0; sourceFace < 6; ++sourceFace)
	{
		cresult = envMap->GetSubResourceData(faceArray[sourceFace], 0, Ceng::IMAGE_FORMAT::fp32_abgr, sourceMap.faceData[sourceFace]);
		if (cresult != Ceng::CE_OK)
		{
			free(solidAngleRayDir);
			return EngineResult::fail;
		}
	}

	end = Ceng_HighPrecisionTimer();

	text = "source copy. t = ";
	text += end - start;
	Ceng::Log::Print(text);

	CubemapData destMap(destWidthInt);

	IrradianceThreadCommon common;

	common.solidAngleRayDir = solidAngleRayDir;
	common.solidAngleRayDirQuadrant = solidAngleRayDirQuadrant;
	common.solidAngleOnly = solidAngleOnly;

	common.faceMatrix = faceMatrix;
	common.faceTranspose = faceTranspose;
	common.invDestWidth = invDestWidth;
	common.invSourceWidth = invSourceWidth;
	common.sourceMap = &sourceMap;
	common.destMap = &destMap;
	common.quadrantWidth = sourceMap.width >> 1;

	EngineResult::value eresult;

	start = Ceng_HighPrecisionTimer();

	//eresult = IrradianceConvolution_v0(common, irradianceMap);
	//eresult = IrradianceConvolution_v0b(common, irradianceMap);
	//eresult = IrradianceConvolution_v0c(common, irradianceMap);
	//eresult = IrradianceConvolution_v0d(common, irradianceMap);
	//eresult = IrradianceConvolution_v0e(common, irradianceMap);
	//eresult = IrradianceConvolution_v0e_b(common, irradianceMap);
	//eresult = IrradianceConvolution_v0e_c(common, irradianceMap);
	//eresult = IrradianceConvolution_v0e_d(common, irradianceMap);
	//eresult = IrradianceConvolution_v0e_e(common, irradianceMap);
	eresult = IrradianceConvolution_v0e_e_m(common, irradianceMap);

	//eresult = IrradianceConvolution_v1(common, irradianceMap);
	//eresult = IrradianceConvolution_v2(common, irradianceMap);
	//eresult = IrradianceConvolution_v3(common, irradianceMap);
	//eresult = IrradianceConvolution_v3b(common, irradianceMap);
	//eresult = IrradianceConvolution_v4(common, irradianceMap);

	end = Ceng_HighPrecisionTimer();

	text = "exterior duration = ";
	text += end - start;
	Ceng::Log::Print(text);

	free(solidAngleRayDir);
	free(solidAngleRayDirQuadrant);

	free(solidAngleOnly);
	free(solidAngleOnlyQuadrant);

	eresult = EngineResult::ok;

	// Copy results to GPU

	for (Ceng::UINT32 destFace = 0; destFace < 6; ++destFace)
	{
		Ceng::CRESULT cresult = irradianceMap->SetSubResourceData(faceArray[destFace], 0, Ceng::IMAGE_FORMAT::fp32_abgr, destMap.faceData[destFace]);
		if (cresult != Ceng::CE_OK)
		{
			eresult = EngineResult::fail;
			break;
		}
	}
	
	return eresult;
}

void CEngine::RayDir(const Ceng::UINT32 u, const Ceng::UINT32 v, const Ceng::CubemapFace::value face,
	const Ceng::FLOAT32 invCubeWidth, Vec3 *out_dir)
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
		out_dir->x = m;
		out_dir->y = -tc;
		out_dir->z = -sc;
		break;
	case Ceng::CubemapFace::negative_x:
		out_dir->x = -m;
		out_dir->y = -tc;
		out_dir->z = sc;
		break;
	case Ceng::CubemapFace::positive_y:
		out_dir->x = sc;
		out_dir->y = m;
		out_dir->z = tc;
		break;
	case Ceng::CubemapFace::negative_y:
		out_dir->x = sc;
		out_dir->y = -m;
		out_dir->z = -tc;
		break;
	case Ceng::CubemapFace::positive_z:
		out_dir->x = sc;
		out_dir->y = -tc;
		out_dir->z = m;
		break;
	case Ceng::CubemapFace::negative_z:
		out_dir->x = -sc;
		out_dir->y = -tc;
		out_dir->z = -m;
		break;
	}
}


void CEngine::RayDir(const Ceng::UINT32 u, const Ceng::UINT32 v, const Ceng::CubemapFace::value face,
	const Ceng::FLOAT32 invCubeWidth, Vec4* out_dir)
{
	Ceng::FLOAT32 s = u * invCubeWidth;
	Ceng::FLOAT32 t = v * invCubeWidth;

	// Adjust to [-1,1] range

	s = 2.0f * s - 1.0f;

	t = 2.0f * t - 1.0f;

	Ceng::FLOAT32 sc, tc, m;

	// Translation from input vector,
	// where m = major axis component of the vector
	//s = sc / abs(m);
	//t = tc / abs(m);

	// Assume input vector is unit length, so
	// dot = sc*sc + tc*tc + m*m = s*s*m*m + t*t*m*m + m*m
	// dot = m*m(1+s*s+t*t) = 1

	m = sqrt(1.0f / (1.0f + s * s + t * t));

	sc = s * m;
	tc = t * m;

	switch (face)
	{
	case Ceng::CubemapFace::positive_x:
		out_dir->x = m;
		out_dir->y = -tc;
		out_dir->z = -sc;
		break;
	case Ceng::CubemapFace::negative_x:
		out_dir->x = -m;
		out_dir->y = -tc;
		out_dir->z = sc;
		break;
	case Ceng::CubemapFace::positive_y:
		out_dir->x = sc;
		out_dir->y = m;
		out_dir->z = tc;
		break;
	case Ceng::CubemapFace::negative_y:
		out_dir->x = sc;
		out_dir->y = -m;
		out_dir->z = -tc;
		break;
	case Ceng::CubemapFace::positive_z:
		out_dir->x = sc;
		out_dir->y = -tc;
		out_dir->z = m;
		break;
	case Ceng::CubemapFace::negative_z:
		out_dir->x = -sc;
		out_dir->y = -tc;
		out_dir->z = -m;
		break;
	}

	out_dir->w = 0.0f;
}
