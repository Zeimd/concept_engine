/*****************************************************************************
*
* lighting-pass.cpp
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#include "lighting-pass.h"
#include "screen-quad.h"
#include "gbuffer.h"
#include "env-map-manager.h"
#include "camera.h"

using namespace CEngine;

LightingPass::LightingPass()
{

}

LightingPass::LightingPass(Ceng::BlendState* lightBlendState, Ceng::DepthStencilState* lightDepthState,
	std::shared_ptr<EnvMapManager>& envMapManager)
	: lightBlendState(lightBlendState), lightDepthState(lightDepthState), envMapManager(envMapManager)
{

}

LightingPass::~LightingPass()
{
	lightBlendState->Release();
	lightDepthState->Release();
}

EngineResult::value LightingPass::GetInstance(Ceng::RenderDevice* renderDevice, std::shared_ptr<EnvMapManager>& envMapManager,
	LightingPass** output)
{
	*output = nullptr;

	Ceng::BlendStateDesc lightBlendDesc;
	Ceng::RenderTargetBlendDesc lightTargetBlend;

	lightBlendDesc.alphaToCoverage = false;
	lightBlendDesc.independentBlend = false;
	lightBlendDesc.targets = 1;
	lightBlendDesc.descList = &lightTargetBlend;

	lightTargetBlend.blendEnable = true;

	lightTargetBlend.sourceBlend = Ceng::BlendType::one;
	lightTargetBlend.destBlend = Ceng::BlendType::one;

	lightTargetBlend.blendOp = Ceng::BlendOp::add;

	lightTargetBlend.sourceBlendAlpha = Ceng::BlendType::one;
	lightTargetBlend.destBlendAlpha = Ceng::BlendType::zero;
	lightTargetBlend.blendAlphaOp = Ceng::BlendOp::add;

	Ceng::BlendState* lightBlendState;

	Ceng::CRESULT cresult;

	cresult = renderDevice->CreateBlendState(&lightBlendDesc, &lightBlendState);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	Ceng::DepthStencilDesc depthDesc;

	depthDesc.depthEnable = true;
	depthDesc.depthWrite = false;
	depthDesc.depthTest = Ceng::TEST_TYPE::NOT_EQUAL;

	Ceng::DepthStencilState* lightDepthState;

	cresult = renderDevice->CreateDepthStencilState(depthDesc, &lightDepthState);
	if (cresult != Ceng::CE_OK)
	{
		lightBlendState->Release();
		return EngineResult::fail;
	}

	LightingPass* temp;

	try
	{
		temp = new LightingPass(lightBlendState, lightDepthState, envMapManager);
	}
	catch (std::bad_alloc&)
	{
		lightBlendState->Release();
		lightDepthState->Release();

		return EngineResult::out_of_memory;
	}

	*output = temp;

	return EngineResult::ok;
}

void LightingPass::Render(Ceng::RenderContext* renderContext, Gbuffer* gbuffer, FullScreenQuad* quad,
	Ceng::SamplerState* nearestSampler, Camera* camera,
	DeferredPassCommonParams* deferredParams, EnvMapCommonParams* envMapParams)
{
	// Disable depth writes
	renderContext->SetDepthStencilState(lightDepthState);

	// Render targets

	renderContext->SetRenderTarget(0, gbuffer->hdrTarget);
	renderContext->SetRenderTarget(1, nullptr);
	renderContext->SetRenderTarget(2, nullptr);

	// Full screen quad vertex data
	renderContext->SetVertexFormat(quad->quadVertexFormat);
	renderContext->SetIndexBuffer(quad->quadIndices);
	renderContext->SetVertexStream(0, quad->quadVertexBuffer, sizeof(QuadVertex), 0);

	// G-buffer inputs

	renderContext->SetPixelShaderResource(0, gbuffer->gbufferColorView);
	renderContext->SetPixelShaderSamplerState(0, nearestSampler);

	renderContext->SetPixelShaderResource(1, gbuffer->gbufferNormalView);
	renderContext->SetPixelShaderSamplerState(1, nearestSampler);

	renderContext->SetPixelShaderResource(2, gbuffer->depthView);
	renderContext->SetPixelShaderSamplerState(2, nearestSampler);

	// Additive blend mode

	renderContext->SetBlendState(lightBlendState, nullptr);

	// Environment probes

	envMapManager->Render(renderContext, deferredParams, envMapParams);

}

