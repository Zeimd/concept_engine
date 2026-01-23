#include "BasicPixelShader.h"

using namespace CEngine;

BasicPixelShader::BasicPixelShader()
{

}

BasicPixelShader::~BasicPixelShader()
{

}

void BasicPixelShader::Release()
{
	delete this;
}

CRESULT BasicPixelShader::ConfigureInput(PixelShaderInputDesc* inputs, Ceng::UINT32 amount)
{
	return CE_ERR_UNIMPLEMENTED;
}

CRESULT BasicPixelShader::ConfigureOutput(PixelShaderOutputDesc* outputs, Ceng::UINT32 amount)
{
	return CE_ERR_UNIMPLEMENTED;
}

CRESULT BasicPixelShader::SetFragmentFormat(const PixelShaderInputDesc* inputs, Ceng::UINT32 inputAmount,
	const PixelShaderOutputDesc* outputs, Ceng::UINT32 outputAmount)
{
	return CE_ERR_UNIMPLEMENTED;
}

CRESULT BasicPixelShader::SetRenderTargets(const PixelShaderOutputDesc* outputs, Ceng::UINT32 amount)
{
	return CE_ERR_UNIMPLEMENTED;
}

CRESULT BasicPixelShader::ProcessQuads(QuadData* quads, Ceng::UINT32 amount)
{
	return CE_ERR_UNIMPLEMENTED;
}