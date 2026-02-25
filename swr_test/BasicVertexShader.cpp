#include "BasicVertexShader.h"

using namespace CEngine;

BasicVertexShader::BasicVertexShader(Ceng::UINT32 cacheLine, Ceng::Vshader::CR_VertexShaderInput* nullInput)
{

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

Ceng::CRESULT BasicVertexShader::Configure(const Ceng::VertexShaderInputDesc* inputSemantics,
	Ceng::UINT32 inputCount, Ceng::UINT32 fragmentSizeBytes,
	Ceng::POINTER inputBaseAddress, Ceng::POINTER inputSteps)
{
	return Ceng::CE_OK;
}

Ceng::CRESULT BasicVertexShader::ProcessVertexBatch(Ceng::UINT32 vertexCount,
	Ceng::FragmentCacheTag* vertexIndex,
	Ceng::UINT8* output,
	Ceng::UINT32 threadId)
{
	return Ceng::CE_OK;
}