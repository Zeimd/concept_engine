/*****************************************************************************
*
* env-map-manager.cpp
*
* By Jari Korkala 10/2022
*
*****************************************************************************/

#include <ceng.h>

#include "env-map-manager.h"
#include "env-map.h"
#include "shader-manager.h"
#include "texture-manager.h"

using namespace CEngine;

EnvMapManager::EnvMapManager()
{

}

EngineResult::value EnvMapManager::GetInstance(Ceng::RenderDevice* renderDevice, TextureManager* textureManager, ShaderManager* shaderManager,
	EnvMapManager** output)
{
	Ceng::SamplerState* diffuseSampler;

	Ceng::SamplerStateDesc samplerDesc;

	samplerDesc.addressU = Ceng::TextureAddressMode::wrap;
	samplerDesc.addressV = Ceng::TextureAddressMode::wrap;
	samplerDesc.addressW = Ceng::TextureAddressMode::wrap;

	samplerDesc.minFilter = Ceng::TextureMinFilter::linear_mip_linear;
	samplerDesc.magFilter = Ceng::TextureMagFilter::linear;

	Ceng::CRESULT cresult;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &diffuseSampler);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	EnvMapManager* manager = new EnvMapManager();

	manager->renderDevice = renderDevice;
	manager->textureManager = textureManager;
	manager->shaderManager = shaderManager;

	manager->diffuseSampler = diffuseSampler;

	*output = manager;

	return EngineResult::ok;
}

EnvMapManager::~EnvMapManager()
{
	diffuseSampler->Release();
}

EngineResult::value EnvMapManager::AddEnvMapParallaxAABB(const Ceng::StringUtf8& cubemapFile, Vec3 boundaryPos, Vec3 boxSideHalf)
{
	Ceng::Log::Print("EnvMapManager::AddEnvMapParallaxAABB: loading cubemap: ");
	Ceng::Log::Print(cubemapFile);
	Ceng::Log::Print("\n");

	CEngine::EngineResult::value eresult;

	std::shared_ptr<CEngine::ShaderProgram> lightProbeProg;

	std::vector<Ceng::StringUtf8> envFsFlags;

	envFsFlags.push_back("ENVMAP_PARALLAX_AA_BOX");

	eresult = shaderManager->CreateProgramFromFile("quad.vs", nullptr, "light-probe.fs", &envFsFlags, lightProbeProg);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("CreateProgram failed:");

		Ceng::Log::Print(lightProbeProg->managerLog);

		return EngineResult::fail;
	}

	Ceng::ShaderProgram* shaderProgram = lightProbeProg->GetProgram();

	if (shaderProgram == nullptr)
	{
		Ceng::Log::Print("CreateProgram failed:");

		Ceng::Log::Print(lightProbeProg->managerLog);

		return EngineResult::fail;
	}

	if (!shaderProgram->Linked())
	{
		Ceng::Log::Print("Shader Program Log:\n");

		Ceng::StringUtf8* errorLog;

		shaderProgram->GetLog(&errorLog);
		Ceng::Log::Print(*errorLog);

		delete errorLog;

		return EngineResult::fail;
	}

	

	CEngine::TextureOptions defaultTexOptions;

	defaultTexOptions.bindFlags = Ceng::BufferBinding::shader_resource;
	defaultTexOptions.usage = Ceng::BufferUsage::gpu_read_only;
	defaultTexOptions.cpuAccessFlags = 0;

	defaultTexOptions.sRGB = true;

	defaultTexOptions.firstMip = 0;
	defaultTexOptions.mipLevels = 0;

	defaultTexOptions.options = Ceng::BufferOptions::generate_mip_maps;

	defaultTexOptions.generateIrradianceMap = true;
	defaultTexOptions.irradianceSize = 16;

	std::shared_ptr<CEngine::Texture> envMapHandle, irradianceHandle;

	eresult = textureManager->LoadCubemap(cubemapFile, defaultTexOptions, envMapHandle, irradianceHandle);
	if (eresult != CEngine::EngineResult::ok)
	{
		return EngineResult::fail;
	}

	Ceng::Log::Print("Cubemap loaded\n");

	std::shared_ptr<EnvProbeShaderParallaxAABB> envProbeShader;
	
	bool found = false;

	for (auto& shader : shaders)
	{
		if (shader->program == lightProbeProg)
		{
			envProbeShader = std::static_pointer_cast<EnvProbeShaderParallaxAABB>(shader);
			found = true;
			break;
		}
	}

	if (!found)
	{
		//envProbeShader = std::make_shared< EnvProbeShaderParallaxAABB>();

		EnvProbeShaderParallaxAABB* temp;

		eresult = EnvProbeShaderParallaxAABB::GetInstance(lightProbeProg, &temp);

		if (eresult != EngineResult::ok)
		{
			Ceng::Log::Print("EnvProbeShaderParallaxAABB::GetInstance failed\n");
			return eresult;
		}

		envProbeShader.reset(temp);

		shaders.push_back(envProbeShader);
	}

	envProbeShader->program = lightProbeProg;

	std::shared_ptr<EnvProbeAABOX> envProbe = std::make_shared<EnvProbeAABOX>();

	Ceng::ShaderResourceViewDesc envViewDesc;

	envViewDesc.cubeMap.baseMipLevel = 0;
	envViewDesc.cubeMap.maxMipLevel = 1;

	Ceng::CRESULT cresult;

	Ceng::ShaderResourceView* envMapView;

	cresult = envMapHandle->AsCubemap()->GetShaderViewCubemap(envViewDesc, &envMapView);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	Ceng::ShaderResourceView* irradianceMapView;

	cresult = irradianceHandle->AsCubemap()->GetShaderViewCubemap(envViewDesc, &irradianceMapView);
	if (cresult != Ceng::CE_OK)
	{
		envMapView->Release();
		return EngineResult::fail;
	}

	envProbe->name = cubemapFile;
	envProbe->envMap = envMapHandle;
	envProbe->irradianceMap = irradianceHandle;
	envProbe->envMapView = envMapView;
	envProbe->irradianceMapView = irradianceMapView;

	envProbe->boundaryCenterWorldPos = boundaryPos;
	envProbe->boxSideHalf = boxSideHalf;

	envProbe->program = envProbeShader;

	probes.push_back(envProbe);



	return EngineResult::ok;
}

void EnvMapManager::Render(Ceng::RenderContext* renderContext, DeferredPassCommonParams* deferredParams, EnvMapCommonParams* envMapParams)
{
	Ceng::Matrix4 reverseCameraRotation = envMapParams->cameraReverseRotation;

	for (auto& envShader : shaders)
	{
		//renderContext->SetShaderProgram(envShader->program->GetProgram());

		envShader->fs_windowWidth->SetFloat((Ceng::FLOAT32)deferredParams->windowWidth);
		envShader->fs_windowHeight->SetFloat((Ceng::FLOAT32)deferredParams->windowHeight);

		envShader->fs_xDilationDiv->SetFloat(deferredParams->xDilationDiv);
		envShader->fs_yDilationDiv->SetFloat(deferredParams->yDilationDiv);
		envShader->fs_zTermA->SetFloat(deferredParams->zTermA);
		envShader->fs_zTermB->SetFloat(deferredParams->zTermB);

		envShader->fs_gbufferColor->SetInt(deferredParams->gbufferColorSlot);
		envShader->fs_gbufferNormal->SetInt(deferredParams->gbufferNormalSlot);
		envShader->fs_depthBuffer->SetInt(deferredParams->depthBufferSlot);

		envShader->fs_reflectionEnv->SetInt(envMapParams->envMapSlot);
		envShader->fs_diffuseEnv->SetInt(envMapParams->irradianceSlot);

		envShader->fs_cameraReverse->SetMatrix_4x4(&reverseCameraRotation.data[0][0], true);

		renderContext->SetPixelShaderSamplerState(envMapParams->envMapSlot, diffuseSampler);
		renderContext->SetPixelShaderSamplerState(envMapParams->irradianceSlot, diffuseSampler);

	}

	for (auto& envProbe : probes)
	{
		CEngine::ShaderProgram* program = envProbe->GetProgram()->program.get();

		renderContext->SetShaderProgram(program->GetProgram());

		envProbe->PrepareRender(envMapParams->cameraWorldPos);

		Ceng::BufferData2D data;

		envProbe->envMap->AsCubemap()->GetBufferData2D(&data);

		envProbe->GetProgram()->fs_maxEnvLOD->SetFloat(Ceng::FLOAT32(data.mipLevels));

		renderContext->SetPixelShaderResource(3, envProbe->envMapView);
		renderContext->SetPixelShaderResource(4, envProbe->irradianceMapView);

		/*
		renderContext->SetPixelShaderResource(3, envProbe->envMapView);
		renderContext->SetPixelShaderSamplerState(3, diffuseSampler);

		renderContext->SetPixelShaderResource(4, envProbe->irradianceMapView);
		renderContext->SetPixelShaderSamplerState(4, diffuseSampler);
		*/


		renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
	}
}