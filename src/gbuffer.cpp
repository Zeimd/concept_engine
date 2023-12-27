/*****************************************************************************
*
* gbuffer.cpp
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#include "gbuffer.h"

using namespace CEngine;

Gbuffer::Gbuffer()
{

}

Gbuffer::Gbuffer(Ceng::Texture2D* gbufferColor, Ceng::ShaderResourceView* gbufferColorView, Ceng::RenderTargetView* gbufferColorTarget,
	Ceng::Texture2D* gbufferNormal, Ceng::ShaderResourceView* gbufferNormalView, Ceng::RenderTargetView* gbufferNormalTarget,
	Ceng::Texture2D* depthTexture, Ceng::RenderTargetView* depthTarget, Ceng::ShaderResourceView* depthView,
	Ceng::Texture2D* hdrTexture, Ceng::ShaderResourceView* hdrView, Ceng::RenderTargetView* hdrTarget)

	: gbufferColor(gbufferColor), gbufferColorView(gbufferColorView), gbufferColorTarget(gbufferColorTarget),
	gbufferNormal(gbufferNormal), gbufferNormalView(gbufferNormalView), gbufferNormalTarget(gbufferNormalTarget),
	depthTexture(depthTexture), depthTarget(depthTarget),depthView(depthView),
	hdrTexture(hdrTexture), hdrView(hdrView), hdrTarget(hdrTarget)
{

}


Gbuffer::~Gbuffer()
{
	gbufferColorView->Release();
	gbufferColorTarget->Release();

	gbufferColor->Release();
	
	gbufferNormalView->Release();
	gbufferNormalTarget->Release();

	gbufferNormal->Release();

	depthTarget->Release();
	depthView->Release();

	depthTexture->Release();

}

EngineResult::value Gbuffer::GetInstance(Ceng::RenderDevice* renderDevice, Ceng::UINT32 width, Ceng::UINT32 height, Gbuffer** output)
{
	*output = nullptr;

	Ceng::Texture2dDesc gbufferDesc;

	gbufferDesc.width = width;
	gbufferDesc.height = height;

	gbufferDesc.format = Ceng::IMAGE_FORMAT::C32_ABGR;

	gbufferDesc.arraySize = 1;
	gbufferDesc.mipLevels = 1;

	gbufferDesc.sRGB = false;

	gbufferDesc.multisampleDesc.count = 0;
	gbufferDesc.multisampleDesc.quality = 0;

	gbufferDesc.bindFlags = Ceng::BufferBinding::shader_resource | Ceng::BufferBinding::render_target;
	gbufferDesc.usage = Ceng::BufferUsage::gpu_read_write;

	gbufferDesc.cpuAccessFlags = 0;

	gbufferDesc.optionFlags = 0;

	Ceng::RenderTargetViewDesc targetDesc;

	targetDesc.type = Ceng::BufferType::texture_2d;
	targetDesc.tex2d.mipSlice = 0;

	Ceng::ShaderResourceViewDesc diffuseViewDesc;

	diffuseViewDesc.format = Ceng::IMAGE_FORMAT::C24_BGR;
	diffuseViewDesc.tex2d.baseMipLevel = 0;
	diffuseViewDesc.tex2d.maxMipLevel = -1;

	Ceng::Texture2D* gbufferColor;
	Ceng::ShaderResourceView* gbufferColorView;
	Ceng::RenderTargetView* gbufferColorTarget;

	Ceng::CRESULT cresult;

	cresult = renderDevice->CreateTexture2D(gbufferDesc, nullptr, &gbufferColor);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng::Log::Print("Error: Failed to create G-buffer color texture : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng::Log::Print("Error: Failed to create G-buffer color texture : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng::Log::Print("Error: Failed to create G-buffer color texture : Out of video memory");
			break;
		default:
			Ceng::Log::Print("Error: Failed to create G-buffer color texture : unknown error");
			break;
		};
		return EngineResult::fail;
	}

	cresult = gbufferColor->GetRenderTargetView(targetDesc, &gbufferColorTarget);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng::Log::Print("Error: Failed to create G-buffer color target : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng::Log::Print("Error: Failed to create G-buffer color target : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng::Log::Print("Error: Failed to create G-buffer color target : Out of video memory");
			break;
		default:
			Ceng::Log::Print("Error: Failed to create G-buffer color target : unknown error");
			break;
		};

		gbufferColor->Release();
		return EngineResult::fail;
	}

	cresult = gbufferColor->GetShaderViewTex2D(diffuseViewDesc, &gbufferColorView);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng::Log::Print("Error: Failed to create G-buffer color view : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng::Log::Print("Error: Failed to create G-buffer color view : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng::Log::Print("Error: Failed to create G-buffer color view : Out of video memory");
			break;
		default:
			Ceng::Log::Print("Error: Failed to create G-buffer color view : unknown error");
			break;
		};

		gbufferColor->Release();
		gbufferColorTarget->Release();

		return EngineResult::fail;
	}

	// Use float16 format to store normals with high precision
	gbufferDesc.format = Ceng::IMAGE_FORMAT::CF16_ABGR;

	Ceng::Texture2D* gbufferNormal;
	Ceng::ShaderResourceView* gbufferNormalView;
	Ceng::RenderTargetView* gbufferNormalTarget;

	cresult = renderDevice->CreateTexture2D(gbufferDesc, nullptr, &gbufferNormal);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng::Log::Print("Error: Failed to create G-buffer normal surface : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng::Log::Print("Error: Failed to create G-buffer normal surface : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng::Log::Print("Error: Failed to create G-buffer normal surface : Out of video memory");
			break;
		default:
			Ceng::Log::Print("Error: Failed to create G-buffer normal surface : unknown error");
			break;
		};

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		return EngineResult::fail;
	}

	cresult = gbufferNormal->GetRenderTargetView(targetDesc, &gbufferNormalTarget);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng::Log::Print("Error: Failed to create G-buffer normal target : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng::Log::Print("Error: Failed to create G-buffer normal target :  API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng::Log::Print("Error: Failed to create G-buffer normal target : Out of video memory");
			break;
		default:
			Ceng::Log::Print("Error: Failed to create G-buffer normal target : unknown error");
			break;
		};

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();

		return EngineResult::fail;
	}

	cresult = gbufferNormal->GetShaderViewTex2D(diffuseViewDesc, &gbufferNormalView);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng::Log::Print("Error: Failed to create G-buffer normal view : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng::Log::Print("Error: Failed to create G-buffer normal view : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng::Log::Print("Error: Failed to create G-buffer normal view : Out of video memory");
			break;
		default:
			Ceng::Log::Print("Error: Failed to create G-buffer normal view : unknown error");
			break;
		};

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();

		return EngineResult::fail;
	}

	Ceng::Texture2dDesc depthDesc;

	depthDesc.width = width;
	depthDesc.height = height;

	//depthDesc.format = Ceng::IMAGE_FORMAT::D24_S8;
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

	Ceng::Texture2D* depthTexture;
	Ceng::RenderTargetView* depthTarget;
	Ceng::ShaderResourceView* depthView;

	cresult = renderDevice->CreateTexture2D(depthDesc, nullptr, &depthTexture);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create depth buffer");

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		return EngineResult::fail;
	}

	cresult = depthTexture->GetRenderTargetView(targetDesc, &depthTarget);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create depth target");

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		depthTexture->Release();

		return EngineResult::fail;
	}


	cresult = depthTexture->GetShaderViewTex2D(diffuseViewDesc, &depthView);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create depth texture view");

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		depthTexture->Release();
		depthTarget->Release();

		return EngineResult::fail;
	}

	Ceng::Texture2dDesc hdrDesc;

	hdrDesc.width = width;
	hdrDesc.height = height;

	hdrDesc.format = Ceng::IMAGE_FORMAT::CF16_ABGR;
	//hdrDesc.format = Ceng::IMAGE_FORMAT::CF32_ABGR;
	//hdrDesc.format = Ceng::IMAGE_FORMAT::C32_ABGR;

	hdrDesc.mipLevels = 1;
	hdrDesc.arraySize = 1;

	hdrDesc.sRGB = false;

	hdrDesc.multisampleDesc.count = 0;
	hdrDesc.multisampleDesc.quality = 0;

	hdrDesc.bindFlags = Ceng::BufferBinding::shader_resource | Ceng::BufferBinding::render_target;
	hdrDesc.usage = Ceng::BufferUsage::gpu_read_write;
	hdrDesc.cpuAccessFlags = 0;
	hdrDesc.optionFlags = 0;

	Ceng::Texture2D* hdrTexture;
	Ceng::ShaderResourceView* hdrView;
	Ceng::RenderTargetView* hdrTarget;

	cresult = renderDevice->CreateTexture2D(hdrDesc, nullptr, &hdrTexture);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create HDR texture");

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		depthTexture->Release();
		depthTarget->Release();
		depthView->Release();

		return EngineResult::fail;
	}


	cresult = hdrTexture->GetShaderViewTex2D(diffuseViewDesc, &hdrView);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create HDR texture view");

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		depthTexture->Release();
		depthTarget->Release();
		depthView->Release();

		hdrTexture->Release();

		return EngineResult::fail;
	}


	cresult = hdrTexture->GetRenderTargetView(targetDesc, &hdrTarget);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create HDR texture target");

		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		depthTexture->Release();
		depthTarget->Release();
		depthView->Release();

		hdrTexture->Release();
		hdrView->Release();

		return EngineResult::fail;
	}

	Gbuffer* temp;

	try
	{
		temp = new Gbuffer(gbufferColor, gbufferColorView, gbufferColorTarget, 
			gbufferNormal, gbufferNormalView, gbufferNormalTarget,
			depthTexture, depthTarget, depthView,
			hdrTexture, hdrView, hdrTarget);
	}
	catch (std::bad_alloc&)
	{
		gbufferColor->Release();
		gbufferColorTarget->Release();
		gbufferColorView->Release();

		gbufferNormal->Release();
		gbufferNormalTarget->Release();
		gbufferNormalView->Release();

		depthTexture->Release();
		depthTarget->Release();
		depthView->Release();

		hdrTexture->Release();
		hdrView->Release();
		hdrTarget->Release();

		return EngineResult::out_of_memory;
	}

	*output = temp;

	return EngineResult::ok;
}
