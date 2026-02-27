#include "BasicVertexShader.h"

using namespace CEngine;

BasicVertexShader::BasicVertexShader(Ceng::UINT32 cacheLine, Ceng::Vshader::CR_VertexShaderInput* nullInput,
	Ceng::Vshader::CR_VertexShaderOutput* nullOutput)
{
	outputBaseAddress = 0;

	inputRegisters[0].variable = &inPosition;
	inputRegisters[0].type = Ceng::SHADER_DATATYPE::FLOAT4;

	inputRegisters[1].variable = &inNormal;
	inputRegisters[1].type = Ceng::SHADER_DATATYPE::FLOAT4;

	inputRegisters[2].variable = &inTangent;
	inputRegisters[2].type = Ceng::SHADER_DATATYPE::FLOAT4;

	inputRegisters[3].variable = &inTexCoord0;
	inputRegisters[3].type = Ceng::SHADER_DATATYPE::FLOAT2;

	inputRegisters[4].variable = &inTexCoord1;
	inputRegisters[4].type = Ceng::SHADER_DATATYPE::FLOAT2;

	for (int k = 0; k < outputRegisters.size(); ++k)
	{
		inputRegisters[k].variable->callBack = nullInput->callBack;
		inputRegisters[k].variable->sourceAddress = nullInput->sourceAddress;
	}

	outputRegisters[0].variable = &outPosition;
	outputRegisters[1].variable = &outNormal;
	outputRegisters[2].variable = &outTangent;
	outputRegisters[3].variable = &outTexCoord0;
	outputRegisters[4].variable = &outTexCoord1;

	for (int k = 0; k < outputRegisters.size(); ++k)
	{
		outputRegisters[k].variable->destAddress = nullOutput->destAddress;
		outputRegisters[k].variable->destOffset = nullOutput->destOffset;
	}

	uniforms[0].variable = &fullVertexTransform;
}

void BasicVertexShader::Release()
{
	delete this;
}

Ceng::Vshader::VertexShaderInputRegister* BasicVertexShader::GetInputs()
{
	return &inputRegisters[0];
}

Ceng::UINT32 BasicVertexShader::InputSize()
{
	return inputRegisters.size();
}

Ceng::Vshader::VertexShaderOutputRegister* BasicVertexShader::GetOutputs()
{
	return &outputRegisters[0];
}

Ceng::UINT32 BasicVertexShader::OutputSize()
{
	return outputRegisters.size();
}

Ceng::Vshader::VertexShaderUniform* BasicVertexShader::GetUniforms()
{
	return &uniforms[0];
}

Ceng::UINT32 BasicVertexShader::UniformSize()
{
	return uniforms.size();
}

Ceng::POINTER* BasicVertexShader::OutputBaseAddress()
{
	return &outputBaseAddress;
}

Ceng::CRESULT BasicVertexShader::BasicConfig(Ceng::UINT32 fragmentSizeBytes,
	Ceng::POINTER* inputBaseAddress, Ceng::POINTER* inputSteps)
{
	this->fragmentSizeBytes = fragmentSizeBytes;
	this->inputBaseAddress = inputBaseAddress;
	this->inputSteps = inputSteps;

	return Ceng::CE_OK;
}

Ceng::CRESULT BasicVertexShader::ProcessVertexBatch(Ceng::UINT32 vertexCount,
	Ceng::FragmentCacheTag* vertexIndex,
	Ceng::UINT8* output,
	Ceng::UINT32 threadId)
{
	outputBaseAddress = (Ceng::POINTER)output;

	for (Ceng::UINT32 k = 0; k < vertexCount; k++)
	{
		Ceng::UINT32 index = vertexIndex[k].tag;

		// Move input pointers to correct position

		for (Ceng::INT32 i = 0; i < inputRegisters.size(); i++)
		{
			inputRegisters[i].variable->sourceAddress = inputBaseAddress[i]
				+ index * inputSteps[i];
		}

		ShaderFunction();

		outputBaseAddress += fragmentSizeBytes;
	}	

	return Ceng::CE_OK;
}

void BasicVertexShader::ShaderFunction()
{
	_declspec(align(16)) Ceng::VectorF4 temp;
	_declspec(align(16)) Ceng::VectorF4 positionTemp;

	// NOTE: Use IN_SEMANTIC pointers to obtain
	//       correct variable from input stream

	positionTemp = inPosition;
	positionTemp.w = Ceng::FLOAT32(1.0f);

	positionTemp = fullVertexTransform * positionTemp;

	outPosition = positionTemp;

	// NOTE: Can't transfer directly from input to output
	//       because the temporary Vector4 won't be
	//       aligned in memory

	/*
	temp = *IN_NORMAL;
	*OUT_NORMAL = temp;


	temp = *IN_COLOR0;
	*OUT_COLOR0 = temp;
	*/

	_declspec(align(16)) Ceng::VectorF2 v2Temp;

	v2Temp = inTexCoord0;
	outTexCoord0 = v2Temp;


	v2Temp = inTexCoord1;
	outTexCoord1 = v2Temp;

	/*
	tempFloat = *IN_TEXCOORD2;
	*OUT_TEXCOORD2 = tempFloat;
	*/
}
