#include "BasicPixelShader.h"

using namespace CEngine;

BasicPixelShader::BasicPixelShader(Ceng::UINT32 cacheLine, Pshader::CR_PixelShaderInput* nullInput)
{
	quadTargetStart = 0;

	perspectiveTemp = AlignedBuffer<Ceng::UINT8>(64, cacheLine);

	coverageAddress = NULL;
	stepBufferPtr = NULL;

	inputRegisters[0].variable = &normal;
	inputRegisters[1].variable = &tangent;
	inputRegisters[2].variable = &texCoord0;
	inputRegisters[3].variable = &texCoord1;

	for (int k = 0; k < inputRegisters.size(); ++k)
	{
		*(inputRegisters[k].variable) = *nullInput;
	}

	outputRegisters[0].variable = &OUT_DEPTH;
	outputRegisters[0].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET_DEPTH;

	outputRegisters[1].variable = &OUT_STENCIL;
	outputRegisters[1].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET_STENCIL;

	outputRegisters[1].variable = &OUT_TARGET0;
	outputRegisters[1].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET0;

	outputRegisters[2].variable = &OUT_TARGET0;
	outputRegisters[2].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET0;

	outputRegisters[3].variable = &OUT_TARGET1;
	outputRegisters[3].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET1;

	outputRegisters[4].variable = &OUT_TARGET2;
	outputRegisters[4].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET2;

	outputRegisters[5].variable = &OUT_TARGET3;
	outputRegisters[5].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET3;

	outputRegisters[6].variable = &OUT_TARGET4;
	outputRegisters[6].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET4;

	outputRegisters[7].variable = &OUT_TARGET5;
	outputRegisters[7].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET5;

	outputRegisters[8].variable = &OUT_TARGET6;
	outputRegisters[8].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET6;

	outputRegisters[9].variable = &OUT_TARGET7;
	outputRegisters[9].target = PSHADER_OUTPUT_SEMANTIC::CR_SHADER_TARGET7;

	uniforms[0].variable = &diffuseTex;
}

BasicPixelShader::~BasicPixelShader()
{

}

void BasicPixelShader::Release()
{
	delete this;
}

CRESULT BasicPixelShader::BasicConfig(Ceng::UINT32 quadSize, Ceng::UINT32 cacheLine, Ceng::POINTER quadTargetStart)
{
	// Allocate space for a quad's varying data

	if (quadBuffer == nullptr)
	{
		quadBuffer = AlignedBuffer<UINT8>(quadSize, cacheLine);
	}

	quadTargetStart = quadTargetStart;

	return CE_OK;
}

Ceng::UINT8* BasicPixelShader::QuadBuffer()
{
	return (UINT8*)quadBuffer;
}

Ceng::POINTER* BasicPixelShader::StepBufferPtr()
{
	return &stepBufferPtr;
}

void* BasicPixelShader::PerpectiveTemp()
{
	return (void*)&perspectiveTemp;
}

Ceng::UINT32* BasicPixelShader::CoverageAddress()
{
	return &coverageAddress;
}


Pshader::PixelShaderInputRegister* BasicPixelShader::GetInputs()
{
	return &inputRegisters[0];
}

Ceng::UINT32 BasicPixelShader::InputSize()
{
	return inputRegisters.size();
}

Pshader::PixelShaderOutputRegister* BasicPixelShader::GetOutputs()
{
	return &outputRegisters[0];
}

Ceng::UINT32 BasicPixelShader::OutputSize()
{
	return outputRegisters.size();
}

Pshader::PixelShaderUniform* BasicPixelShader::GetUniforms()
{
	return &uniforms[0];
}

Ceng::UINT32 BasicPixelShader::UniformSize()
{
	return uniforms.size();
}

void BasicPixelShader::ShaderFunction(const FLOAT32* perspective, const FLOAT32* invertW,
	const Ceng::INT32 coverage, const Ceng::INT32 threadId)
{
	// NOTE: Read input registers / write outputs only once because of
//       automatic pixel stepping

	Pshader::Float2 uvDiffuse;

	uvDiffuse = texCoord0;

	//uvDiffuse = uvDiffuse.yx();

	/*
	Shader::Float2 test;

	test.x = 0.5f;
	test.y = 0.5f;

	uvDiffuse += test;
	*/

	//shaderLocal_temp = *IN_COLOR0;

	Pshader::Float4 shaderLocal_temp;
	Pshader::Float shaderLocal_colorScale;



	//shaderLocal_uvDiffuse = *IN_TEXCOORD0;

	//shaderLocal_colorScale = *IN_TEXCOORD1;

	//shaderLocal_temp *= shaderLocal_colorScale;

	_declspec(align(16)) Ceng::VectorF4 color;

	color.x = 0.0f;
	color.y = 0.0f;
	color.z = 0.0f;
	color.w = 0.0f;

	/*
	switch (threadId)
	{
	case 0:
		color.x = 0.0f;
		color.y = 0.5f;
		color.z = 0.0f;
		break;
	case 1:
		color.x = 0.5f;
		color.y = 0.0f;
		break;
	case 2:
		color.y = 0.0f;
		color.z = 0.5f;
		break;
	case 3:
		color.x = 1.0f;
		color.y = 0.0f;
		color.z = 1.0f;
		break;
	case 4:
		color.x = 0.0f;
		color.y = 0.5f;
		color.z = 1.0f;
		break;
	case 5:
		color.x = 0.5f;
		color.y = 0.5f;
		color.z = 0.0f;
		break;
	case 6:
		color.x = 0.5f;
		color.y = 0.5f;
		color.z = 0.5f;
		break;
	case 7:
		color.x = 0.25f;
		color.y = 0.25f;
		color.z = 0.5f;
		break;
	default:
		break;
	}
	*/

	//OUT_TARGET0 = sample2d(diffuseTex, uvDiffuse);

	Pshader::Float4 diffuseColor = sample2d(diffuseTex, uvDiffuse);

	OUT_TARGET0 = diffuseColor;
}

void BasicPixelShader::ProcessConfig(
	UINT32 quadFloatOffset,
	UINT32 quadDoubleOffset,
	UINT32 quadTargetOffset,
	UINT32 floatBlockSize,
	UINT32 doubleBlockSize)
{
	this->quadFloatOffset = quadFloatOffset;
	this->quadDoubleOffset = quadDoubleOffset;
	this->quadTargetOffset = quadTargetOffset;

	this->floatBlockSize = floatBlockSize;
	this->doubleBlockSize = doubleBlockSize;
}

void BasicPixelShader::SetRenderTargetService(Pshader::RenderTargetService* service)
{
	this->renderTargetService = service;
}

_declspec(align(64)) static const Ceng::INT8 newVertTable[16][4] =
{
	{0,0,0,0} ,
	{-1,0,0,0} ,
	{0,-1,0,0} ,
	{-1,-1,0,0} ,
	{0,0,-1,0} ,
	{-1,0,-1,0} ,
	{0,-1,-1,0} ,
	{-1,-1,-1,0} ,
	{0,0,0,-1} ,
	{-1,0,0,-1} ,
	{0,-1,0,-1} ,
	{-1,-1,0,-1} ,
	{0,0,-1,-1} ,
	{-1,0,-1,-1} ,
	{0,-1,-1,-1} ,
	{-1,-1,-1,-1}
};

_declspec(align(16)) static const Ceng::FLOAT32 allOnes[4] = { 1.0f,1.0f,1.0f,1.0f };

_declspec(align(16)) static const Ceng::FLOAT32 colorScaleVec[4] = { 255.0f,255.0f,255.0f,255.0f };


CRESULT BasicPixelShader::ProcessQuads(SWRender::PixelShaderQuadBatch* batch, Ceng::UINT32 batchSize, 
	SWRender::PixelShaderTriangleData* triangleData, Ceng::UINT32 threadId)
{
	UINT32 k;

	FLOAT32* localPerspective = (FLOAT32*)&perspectiveTemp[0];

	UINT32 chainIndex;

	_declspec(align(16)) VectorF4 packedW;

	FLOAT32* invertW = (FLOAT32*)&packedW;

	UINT8* inputBuffer = quadBuffer;
	UINT8* localCoverage;

	const FLOAT32 colorScaleScalar = FLOAT32(255.0f);

	for (k = 0; k < batchSize; k++)
	{
		SWRender::PixelShaderQuadBatch* quad = &batch[k];

		stepBufferPtr = (POINTER)(triangleData->fragment.variableStep);

		UINT8* localSteps = (UINT8*)stepBufferPtr;

		//**********************************
		// Generate temporary quad

		SWRender::CR_FloatFragment* floatParam = triangleData->fragment.floatBlock;
		SWRender::CR_DoubleFragment* doubleParam = triangleData->fragment.doubleBlock;

		FLOAT32 screenX = FLOAT32(quad->screenX);
		FLOAT32 screenY = FLOAT32(quad->screenY);

		VectorF4 localPackedW = triangleData->packedW;

		packedW += triangleData->packedW_dx * (screenX);
		packedW += triangleData->packedW_dy * (screenY);

		VectorF4* destF4 = (VectorF4*)&quadBuffer[quadFloatOffset];

		__m128 screenVecX = _mm_load1_ps(&screenX);
		__m128 screenVecY = _mm_load1_ps(&screenY);

		for (Ceng::UINT32 i = 0; i < floatBlockSize; ++i)
		{
			__m128 startValue = _mm_load_ps((float*)&floatParam[i].startValue);

			__m128 stepX = _mm_load_ps((float*)&floatParam[i].step_dx);
			__m128 stepY = _mm_load_ps((float*)&floatParam[i].step_dy);

			stepX = _mm_mul_ps(stepX, screenVecX);
			stepY = _mm_mul_ps(stepY, screenVecY);

			startValue = _mm_add_ps(startValue, stepX);
			startValue = _mm_add_ps(startValue, stepY);

			_mm_store_ps((float*)&destF4[i], startValue);
		}

		/*
		for(i=0;i<floatBlockSize;i++)
		{
			destF4[i] = floatParam[i].startValue + (floatParam[i].step_dx*screenX) +
				(floatParam[i].step_dy*screenY);
		}
		*/

		// Write DOUBLE variables for the first quad

		/*
		VectorD2 *destD2 = (VectorD2*)&quadBuffer[quadDoubleOffset];

		for(i=0;i<doubleBlockSize;i++)
		{
			destD2[i] = doubleParam[i].startValue + (doubleParam[i].step_dx*screenX)
				+ (doubleParam[i].step_dy*screenY);
		}
		*/

		// Write render target addresses

		// NOTE: Skip targets 0,1 because shader
		//       doesn't use depth-stencil buffer

		POINTER* target = (POINTER*)&quadBuffer[quadTargetOffset];

		renderTargetService->GenerateAddresses(quad->screenX, quad->screenY, target);

		__m128 allOnesVec = _mm_load_ps((float*)allOnes);

		__m128 quadStepVec = _mm_loadu_ps((float*)&triangleData->packedW_quad_dx);

		__m128 invertVecW = _mm_load_ps((float*)invertW);

		__m128 perspectiveVec;

		if (quad->coverageMask[0] == 0)
		{
			// Set coverage pointer to fully covered

			coverageAddress = 15;

			localCoverage = (UINT8*)&newVertTable[15];

			// Chain contains only fully covered quads

			Ceng::UINT32 chainLength = quad->chainLength;

			Ceng::UINT32 chainPacks = chainLength >> 2;
			Ceng::UINT32 chainRemainder = chainLength & 3;

			for (chainIndex = 0; chainIndex < chainPacks; chainIndex++)
			{
				// First quad

				perspectiveVec = _mm_rcp_ps(invertVecW);
				//perspectiveVec = _mm_div_ps(allOnesVec,invertVecW);

				invertVecW = _mm_add_ps(invertVecW, quadStepVec);

				_mm_store_ps((float*)localPerspective, perspectiveVec);
				_mm_store_ps((float*)invertW, invertVecW);

				ShaderFunction(&localPerspective[0], &invertW[0], Ceng::INT32(coverageAddress), threadId);

				// Second quad

				perspectiveVec = _mm_rcp_ps(invertVecW);
				//perspectiveVec = _mm_div_ps(allOnesVec, invertVecW);

				invertVecW = _mm_add_ps(invertVecW, quadStepVec);

				_mm_store_ps((float*)localPerspective, perspectiveVec);
				_mm_store_ps((float*)invertW, invertVecW);

				ShaderFunction(&localPerspective[0], &invertW[0], Ceng::INT32(coverageAddress), threadId);

				// Third quad

				perspectiveVec = _mm_rcp_ps(invertVecW);
				//perspectiveVec = _mm_div_ps(allOnesVec, invertVecW);

				invertVecW = _mm_add_ps(invertVecW, quadStepVec);

				_mm_store_ps((float*)localPerspective, perspectiveVec);
				_mm_store_ps((float*)invertW, invertVecW);

				ShaderFunction(&localPerspective[0], &invertW[0], Ceng::INT32(coverageAddress), threadId);

				// Fourth quad

				perspectiveVec = _mm_rcp_ps(invertVecW);
				//perspectiveVec = _mm_div_ps(allOnesVec, invertVecW);

				invertVecW = _mm_add_ps(invertVecW, quadStepVec);

				_mm_store_ps((float*)localPerspective, perspectiveVec);
				_mm_store_ps((float*)invertW, invertVecW);

				ShaderFunction(&localPerspective[0], &invertW[0], Ceng::INT32(coverageAddress), threadId);

			}

			for (chainIndex = 0; chainIndex < chainRemainder; chainIndex++)
			{
				perspectiveVec = _mm_rcp_ps(invertVecW);
				//perspectiveVec = _mm_div_ps(allOnesVec, invertVecW);

				invertVecW = _mm_add_ps(invertVecW, quadStepVec);

				_mm_store_ps((float*)localPerspective, perspectiveVec);
				_mm_store_ps((float*)invertW, invertVecW);

				// Process the quad
				ShaderFunction(&localPerspective[0], &invertW[0], Ceng::INT32(coverageAddress), threadId);
			}
		}
		else
		{
			// Partially filled chain

			UINT32 coverageIndex = 0;

			Ceng::UINT32 chainLength = quad->chainLength;

			for (chainIndex = 0; chainIndex < chainLength; chainIndex++)
			{
				perspectiveVec = _mm_div_ps(allOnesVec, invertVecW);

				invertVecW = _mm_add_ps(invertVecW, quadStepVec);

				_mm_store_ps((float*)localPerspective, perspectiveVec);
				_mm_store_ps((float*)invertW, invertVecW);

				// Extract coverage information for the quad

				coverageIndex = quad->coverageMask[chainIndex >> 3] >> (4 * (chainIndex & 7));
				coverageIndex = coverageIndex & 15;

				coverageAddress = coverageIndex;

				// Process the quad
				ShaderFunction(&localPerspective[0], &invertW[0], Ceng::INT32(coverageAddress), threadId);
			}
		}
	}

	return CE_OK;
}

