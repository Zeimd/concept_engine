/*****************************************************************************
*
* depth-pass.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "depth-pass.h"

#include "shader-manager.h"

#include "shader-program.h"

#include "entity.h"

#include "shadowmap-component.h"

#include "rotation-component.h"
#include "position-component.h"

#include "mesh-component.h"

using namespace CEngine;

DepthPass::DepthPass(Ceng::RenderDevice *renderDevice,ShaderManager &shaderManager)
{
	EngineResult::value eresult;

	eresult = shaderManager.CreateProgramFromFile("depth-pass.vs", "depth-pass.fs", program);

	Ceng::CRESULT cresult;

	Ceng::ShaderProgram *shaderProgram = program->GetProgram();

	cresult = shaderProgram->GetConstant("fullTransform", &u_fullTransform);

	rasterizerState.cullingMode = Ceng::CULLING_MODE::BACK;
	rasterizerState.frontClockwise = false;
	rasterizerState.scissorEnable = false;

	Ceng::DepthStencilDesc gbufferDepthDesc;

	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::LESS;
	gbufferDepthDesc.depthWrite = true;
	gbufferDepthDesc.stencilEnable = false;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &depthState);
	if (cresult != Ceng::CE_OK)
	{
		return;
	}
}

DepthPass::~DepthPass()
{
	u_fullTransform->Release();
	depthState->Release();
}

void DepthPass::Render(Ceng::RenderContext *renderContext,
	std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Entity>> &entityDict)
{
	ShadowmapComponent *shadowComp;
	Ceng::Matrix4 cameraFullTransform;

	renderContext->SetRenderTarget(0, nullptr);
	renderContext->SetRenderTarget(1, nullptr);
	renderContext->SetRenderTarget(2, nullptr);

	renderContext->SetShaderProgram(program->GetProgram());
	renderContext->SetBlendState(nullptr, nullptr);
	renderContext->SetRasterizerState(&rasterizerState);
	renderContext->SetDepthStencilState(depthState);

	for (auto iter : entityDict)
	{
		auto entity = *iter.second;

		shadowComp = static_cast<ShadowmapComponent*>(entity["shadowmap"].get());

		if (shadowComp == nullptr) continue;
		
		Ceng::UINT32 size = shadowComp->textureSize;

		renderContext->SetViewport(0, 0, size, size);

		renderContext->SetDepth(shadowComp->depthTarget);
		renderContext->ClearDepth(1.0);

		Ceng::Quaternion temp = shadowComp->rotation->rotation;

		temp.Reciprocal();

		switch (shadowComp->type)
		{
		case ShadowmapComponent::point:
		case ShadowmapComponent::spot:
			cameraFullTransform = Ceng::Matrix4(temp) * shadowComp->position->ReversePositionMatrix();
			shadowComp->lightTransform = cameraFullTransform;
			break;
		case ShadowmapComponent::directional:
			cameraFullTransform = Ceng::Matrix4(temp);
			break;
		}

		DepthRender(renderContext,cameraFullTransform,shadowComp->projection,entityDict);
	}
}

void DepthPass::DepthRender(Ceng::RenderContext *renderContext, Ceng::Matrix4 &cameraFullTransform,
	Ceng::Matrix4 &projection,
	std::unordered_map<Ceng::StringUtf8, std::shared_ptr<Entity>> &entityDict)
{
	MeshComponent *meshComp;

	for (auto &iter : entityDict)
	{
		auto &entity = *iter.second;

		meshComp = static_cast<CEngine::MeshComponent*>(entity["mesh"].get());

		if (meshComp == nullptr) continue;

		meshComp->ShadowRender(renderContext, &cameraFullTransform, &projection,u_fullTransform);
	}
}

