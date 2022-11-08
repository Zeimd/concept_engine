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

#include "env-probe.h"
#include "env-probe-shader.h"

using namespace CEngine;

EnvMapManager::EnvMapManager()
{

}

EnvMapManager::EnvMapManager(Ceng::RenderDevice* renderDevice, TextureManager* textureManager, ShaderManager* shaderManager, Ceng::SamplerState* diffuseSampler)
	: renderDevice(renderDevice), textureManager(textureManager), shaderManager(shaderManager), diffuseSampler(diffuseSampler)
{

}

EngineResult::value EnvMapManager::GetInstance(Ceng::RenderDevice* renderDevice, TextureManager* textureManager, ShaderManager* shaderManager,
	std::shared_ptr<EnvMapManager>& output)
{
	output = nullptr;

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

	output = std::make_shared<EnvMapManager>(renderDevice, textureManager, shaderManager, diffuseSampler);

	return EngineResult::ok;
}

EnvMapManager::~EnvMapManager()
{
	diffuseSampler->Release();
}

EngineResult::value EnvMapManager::LoadShader(const std::vector<Ceng::StringUtf8>* vsFlags, 
	const std::vector<Ceng::StringUtf8>* fsFlags, EnvMapEntry** out_entry, std::shared_ptr<ShaderProgram>& out_program)
{
	*out_entry = nullptr;
	out_program = nullptr;

	Ceng::Log::Print("EnvMapManager::LoadShader");

	EngineResult::value eresult;

	std::shared_ptr<ShaderProgram> lightProbeProg;

	eresult = shaderManager->CreateProgramFromFile("quad.vs", vsFlags, "light-probe.fs", fsFlags, lightProbeProg);
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

	std::shared_ptr<EnvProbeShaderParallaxAABB> envProbeShader;

	EnvMapEntry* entry = nullptr;

	bool found = false;

	for (auto& map : envMaps)
	{
		if (map.shader->program == lightProbeProg)
		{
			entry = &map;
			found = true;
			break;
		}
	}

	if (!found)
	{
		envMaps.push_back(EnvMapEntry());
		entry = &envMaps.back();
	}

	out_program = std::move(lightProbeProg);
	*out_entry = entry;

	return EngineResult::ok;
}

EngineResult::value EnvMapManager::LoadEnvMap(const Ceng::StringUtf8& cubemapFile,
	std::shared_ptr<CEngine::Texture>& out_envMapHandle,
	std::shared_ptr<CEngine::Texture>& out_irradianceHandle,
	Ceng::ShaderResourceView** out_envMapView, Ceng::ShaderResourceView** out_irradianceView)
{
	Ceng::Log::Print("EnvMapManager::LoadEnvMap: loading cubemap: ");

	CEngine::EngineResult::value eresult;

	CEngine::TextureOptions defaultTexOptions;

	defaultTexOptions.bindFlags = Ceng::BufferBinding::shader_resource;
	defaultTexOptions.usage = Ceng::BufferUsage::gpu_read_only;
	defaultTexOptions.cpuAccessFlags = 0;

	defaultTexOptions.sRGB = false;

	defaultTexOptions.firstMip = 0;
	defaultTexOptions.mipLevels = -1;

	defaultTexOptions.options = Ceng::BufferOptions::generate_mip_maps;

	defaultTexOptions.generateIrradianceMap = true;
	defaultTexOptions.irradianceSize = 16;

	out_envMapHandle = nullptr;
	out_irradianceHandle = nullptr;
	
	eresult = textureManager->LoadCubemap(cubemapFile, defaultTexOptions, out_envMapHandle, out_irradianceHandle);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to load cube map");
		return EngineResult::fail;
	}

	Ceng::Log::Print("Cubemap loaded\n");

	*out_envMapView = nullptr;
	*out_irradianceView = nullptr;

	Ceng::ShaderResourceViewDesc envViewDesc;

	envViewDesc.cubeMap.baseMipLevel = 0;
	envViewDesc.cubeMap.maxMipLevel = 1;

	Ceng::CRESULT cresult;

	cresult = out_envMapHandle->AsCubemap()->GetShaderViewCubemap(envViewDesc, out_envMapView);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	cresult = out_irradianceHandle->AsCubemap()->GetShaderViewCubemap(envViewDesc, out_irradianceView);
	if (cresult != Ceng::CE_OK)
	{
		(*out_envMapView)->Release();
		return EngineResult::fail;
	}

	return EngineResult::ok;
}

EngineResult::value EnvMapManager::AddEnvMap(const Ceng::StringUtf8& cubemapFile)
{
	Ceng::Log::Print("EnvMapManager::AddEnvMap: loading cubemap: ");
	Ceng::Log::Print(cubemapFile);
	Ceng::Log::Print("\n");

	CEngine::EngineResult::value eresult;

	std::vector<Ceng::StringUtf8> envFsFlags;

	std::shared_ptr<CEngine::ShaderProgram> lightProbeProg;
	EnvMapEntry* entry = nullptr;

	eresult = LoadShader(nullptr, nullptr, &entry, lightProbeProg);
	if (eresult != EngineResult::ok)
	{
		Ceng::Log::Print("Failed to load env map shader with options:");
		for (auto& item : envFsFlags)
		{
			Ceng::Log::Print(item);
		}

		return eresult;
	}

	std::shared_ptr<CEngine::Texture> envMapHandle, irradianceHandle;

	Ceng::ShaderResourceView* envMapView;
	Ceng::ShaderResourceView* irradianceView;

	eresult = LoadEnvMap(cubemapFile, envMapHandle, irradianceHandle, &envMapView, &irradianceView);
	if (eresult != EngineResult::ok)
	{
		Ceng::Log::Print("Failed to load env map textures");

		return eresult;
	}

	// TODO: check that envmap isn't already loaded

	if (entry->shader == nullptr)
	{
		EnvProbeShader* temp;

		eresult = EnvProbeShader::GetInstance(lightProbeProg, &temp);

		if (eresult != EngineResult::ok)
		{
			Ceng::Log::Print("EnvProbeShaderParallaxAABB::GetInstance failed\n");
			return eresult;
		}

		entry->shader.reset(temp);
	}

	std::shared_ptr<EnvProbe> envProbe = std::make_shared<EnvProbe>();

	envProbe->name = cubemapFile;
	envProbe->envMap = envMapHandle;
	envProbe->irradianceMap = irradianceHandle;
	envProbe->envMapView = envMapView;
	envProbe->irradianceMapView = irradianceView;

	//envProbe->program = std::static_pointer_cast<EnvProbeShader>(entry->shader);

	entry->probes.push_back(envProbe);

	return EngineResult::ok;
}

EngineResult::value EnvMapManager::AddEnvMapParallaxAABB(const Ceng::StringUtf8& cubemapFile, 
	const Vec3& world_cubeGenPos, const Vec3& boxSideHalf)
{
	Ceng::Log::Print("EnvMapManager::AddEnvMapParallaxAABB: loading cubemap: ");
	Ceng::Log::Print(cubemapFile);
	Ceng::Log::Print("\n");

	CEngine::EngineResult::value eresult;

	std::vector<Ceng::StringUtf8> envFsFlags;

	envFsFlags.push_back("ENVMAP_PARALLAX_AA_BOX");

	std::shared_ptr<CEngine::ShaderProgram> lightProbeProg;
	EnvMapEntry* entry = nullptr;

	eresult = LoadShader(nullptr, &envFsFlags, &entry, lightProbeProg);
	if (eresult != EngineResult::ok)
	{
		Ceng::Log::Print("Failed to load env map shader with options:");
		for (auto& item : envFsFlags)
		{
			Ceng::Log::Print(item);
		}

		return eresult;
	}

	std::shared_ptr<CEngine::Texture> envMapHandle, irradianceHandle;

	Ceng::ShaderResourceView* envMapView;
	Ceng::ShaderResourceView* irradianceView;

	eresult = LoadEnvMap(cubemapFile, envMapHandle, irradianceHandle, &envMapView, &irradianceView);
	if (eresult != EngineResult::ok)
	{
		Ceng::Log::Print("Failed to load env map textures");

		return eresult;
	}

	// TODO: check that envmap isn't already loaded

	if (entry->shader == nullptr)
	{
		EnvProbeShaderParallaxAABB* temp;

		eresult = EnvProbeShaderParallaxAABB::GetInstance(lightProbeProg, &temp);

		if (eresult != EngineResult::ok)
		{
			Ceng::Log::Print("EnvProbeShaderParallaxAABB::GetInstance failed\n");
			return eresult;
		}

		entry->shader.reset(temp);
	}

	std::shared_ptr<EnvProbeAABOX> envProbe = std::make_shared<EnvProbeAABOX>();

	envProbe->name = cubemapFile;
	envProbe->envMap = envMapHandle;
	envProbe->irradianceMap = irradianceHandle;
	envProbe->envMapView = envMapView;
	envProbe->irradianceMapView = irradianceView;

	envProbe->world_cubeGenPos = world_cubeGenPos;
	envProbe->boxSideHalf = boxSideHalf;

	envProbe->program = std::static_pointer_cast<EnvProbeShaderParallaxAABB>(entry->shader);

	entry->probes.push_back(envProbe);

	return EngineResult::ok;
}

void EnvMapManager::Render(Ceng::RenderContext* renderContext, 
	DeferredPassCommonParams* deferredParams, EnvMapCommonParams* envMapParams)
{
	for (auto& entry : envMaps)
	{
		CEngine::ShaderProgram* program = entry.shader->program.get();

		renderContext->SetShaderProgram(program->GetProgram());

		entry.shader->fs_windowWidth->SetFloat((Ceng::FLOAT32)deferredParams->windowWidth);
		entry.shader->fs_windowHeight->SetFloat((Ceng::FLOAT32)deferredParams->windowHeight);

		entry.shader->fs_xDilationDiv->SetFloat(deferredParams->xDilationDiv);
		entry.shader->fs_yDilationDiv->SetFloat(deferredParams->yDilationDiv);
		entry.shader->fs_zTermA->SetFloat(deferredParams->zTermA);
		entry.shader->fs_zTermB->SetFloat(deferredParams->zTermB);

		entry.shader->fs_gbufferColor->SetInt(deferredParams->gbufferColorSlot);
		entry.shader->fs_gbufferNormal->SetInt(deferredParams->gbufferNormalSlot);
		entry.shader->fs_depthBuffer->SetInt(deferredParams->depthBufferSlot);

		entry.shader->fs_reflectionEnv->SetInt(envMapParams->envMapSlot);
		entry.shader->fs_diffuseEnv->SetInt(envMapParams->irradianceSlot);

		entry.shader->fs_cameraReverse->SetMatrix_4x4(&envMapParams->cameraReverseRotation.data[0][0], true);

		renderContext->SetPixelShaderSamplerState(envMapParams->envMapSlot, diffuseSampler);
		renderContext->SetPixelShaderSamplerState(envMapParams->irradianceSlot, diffuseSampler);
	
		for (auto& probe : entry.probes)
		{
			probe->PrepareRender(envMapParams->cameraWorldPos);

			Ceng::BufferData2D data;

			probe->envMap->AsCubemap()->GetBufferData2D(&data);

			entry.shader->fs_maxEnvLOD->SetFloat(Ceng::FLOAT32(data.mipLevels));

			renderContext->SetPixelShaderResource(envMapParams->envMapSlot, 
				probe->envMapView);

			renderContext->SetPixelShaderResource(envMapParams->irradianceSlot, 
				probe->irradianceMapView);

			renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
		}
	}
}