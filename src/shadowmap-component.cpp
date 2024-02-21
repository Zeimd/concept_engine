/*****************************************************************************
*
* shadowmap-component.cpp
*
* By Jari Korkala 10/6/2015
*
*****************************************************************************/

#include "shadowmap-component.h"
#include "texture-options.h"

#include "point-light-component.h"
#include "rotation-component.h"
#include "position-component.h"

#include "spot-light-component.h"

using namespace CEngine;

ShadowmapComponent::ShadowmapComponent()
{
}

ShadowmapComponent::ShadowmapComponent(Ceng::RenderDevice *renderDevice, const Ceng::UINT32 textureSize,
	const Ceng::FLOAT32 zNear, const Ceng::FLOAT32 depthBias)
	: light(nullptr), rotation(nullptr), renderDevice(renderDevice), textureSize(textureSize),zNear(zNear),
	depthBias(depthBias)
{
	Ceng::Texture2dDesc depthDesc;

	depthDesc.width = textureSize;
	depthDesc.height = textureSize;

	depthDesc.format = Ceng::IMAGE_FORMAT::D32F;

	depthDesc.mipLevels = 1;
	depthDesc.arraySize = 1;

	depthDesc.sRGB = false;

	depthDesc.multisampleDesc.count = 0;
	depthDesc.multisampleDesc.quality = 0;

	depthDesc.bindFlags = Ceng::BufferBinding::shader_resource | Ceng::BufferBinding::render_target;
	depthDesc.usage = Ceng::BufferUsage::gpu_read_write;
	depthDesc.cpuAccessFlags = 0;
	depthDesc.optionFlags = 0;

	Ceng::CRESULT cresult;

	cresult = renderDevice->CreateTexture2D(depthDesc, nullptr, &depthTexture);
	if (cresult != Ceng::CE_OK)
	{
		return;
	}

	Ceng::RenderTargetViewDesc targetDesc;

	targetDesc.type = Ceng::BufferType::texture_2d;
	targetDesc.tex2d.mipSlice = 0;

	depthTexture->GetRenderTargetView(targetDesc, &depthTarget);


	Ceng::ShaderResourceViewDesc diffuseViewDesc;

	diffuseViewDesc.format = Ceng::IMAGE_FORMAT::unorm_b8_g8_r8;
	diffuseViewDesc.tex2d.baseMipLevel = 0;
	diffuseViewDesc.tex2d.maxMipLevel = -1;

	depthTexture->GetShaderViewTex2D(diffuseViewDesc, &depthView);
}

ShadowmapComponent::~ShadowmapComponent()
{
	depthView->Release();
	depthTarget->Release();
	depthTexture->Release();
}

void ShadowmapComponent::AddComponent(const Ceng::StringUtf8 &name, Component *component)
{
	if (name == "dir_light")
	{
		light = static_cast<PointLightComponent*>(component);
		type = directional;
	}
	else if (name == "spot_light")
	{
		light = static_cast<PointLightComponent*>(component);
	}
	else if (name == "spot_data")
	{
		spotData = static_cast<SpotLightComponent*>(component);
	}
	else if (name == "point_light")
	{
		light = static_cast<PointLightComponent*>(component);
		type = point;
	}
	else if (name == "rotation")
	{
		rotation = static_cast<RotationComponent*>(component);
	}
	else if (name == "position")
	{
		position = static_cast<PositionComponent*>(component);
	}

	if (spotData != nullptr && light != nullptr)
	{
		type = spot;
		
		renderDevice->CreateProjectionMatrix(textureSize, textureSize,
			spotData->outerAngle, zNear, light->cutoffDist,
			&projection);
		
	}
}

void ShadowmapComponent::RemoveComponent(const Ceng::StringUtf8 &name)
{
}