/*****************************************************************************
*
* opengl-test.cpp
*
* Created By Jari Korkala 15/3/2015
*
*****************************************************************************/

#include <ceng.h>

#include <ceng/lib/liblog.h>

#include <ceng/lib/timerlib.h>

#include "engine-v2.h"

#include <upng.h>

const Ceng::String ImageFormatToString(const Ceng::IMAGE_FORMAT::value format);


// Debug information
#ifdef _MSC_VER

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_WARNINGS
#include <crtdbg.h>

#endif

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sstream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{

#ifdef _DEBUG
	// Detect memory leaks after main-function ends
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Ceng::StringUtf8 appPath,currentPath;
	
	Ceng_ExePath(appPath);
	Ceng_CurrentPath(currentPath);

	Ceng::CRESULT cresult;

	Ceng::FileLog engineLog;
	Ceng::Log::SetLog(&engineLog);

	std::stringstream writer;

	cresult = engineLog.OpenFile("opengl-test-log.txt");
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::SetLog(NULL);
		return 0;
	}

	Ceng::Log::Print("Program started");

	Ceng::StringUtf8 temp = "executable path = ";
	temp += appPath;

	Ceng::Log::Print(temp);

	temp = "current path = ";
	temp += currentPath;

	Ceng::Log::Print(temp);

	Ceng::StringUtf8 assetRelativePath = "../assets/";
	Ceng::StringUtf8 assetPath;

	cresult = Ceng_ApplyRelativePath(appPath, assetRelativePath, assetPath);

	temp = "assets path = ";
	temp += assetPath;

	Ceng::Log::Print(temp);

	Ceng::StringUtf8 longPath;

	Ceng_MakeOSPath(assetPath, longPath);

	temp = "windows path = ";
	temp += longPath;

	Ceng::Log::Print(temp);

	/*
	upng_t *png;

	Ceng::StringUtf8 tempPath = longPath + "textures\\normal b.png";

	Ceng::Log::Print(tempPath);

	png = upng_new_from_file(tempPath.ToCString());
	if (png != nullptr) {
		upng_decode(png);
	}

	CEngine::Bitmap bitmap;

	bitmap.width = upng_get_width(png);
	bitmap.height = upng_get_height(png);

	bitmap.imageSize = upng_get_size(png);

	const Ceng::UINT8 *sourcePtr = upng_get_buffer(png);

	// Swap blue and red channels

	FILE *dump = fopen("normal.txt", "w");

	for (Ceng::UINT32 y = 0; y < bitmap.height; ++y)
	{
		for (Ceng::UINT32 x = 0; x < bitmap.width; ++x)
		{
			Ceng::UINT32 offset = 4 * (y*bitmap.width + x);
			fprintf(dump,"{%u,%u,%u},", sourcePtr[offset], sourcePtr[offset + 1], sourcePtr[offset + 2]);
		}
		fprintf(dump, "\n");
	}

	upng_free(png);

	fclose(dump);


	return 0;
	*/
	Ceng::UINT32 resX = 800;
	Ceng::UINT32 resY = 600;

	Ceng::BOOL fullScreenMode = false;

	Ceng::String programName;
	Ceng::String captionInfo;

	programName = "Ceng-gl32 v ";
	programName += CEngine::majorVersion;
	programName += ".";
	programName += CEngine::minorVersion;
	programName += " Build ";
	programName += CEngine::build;

	Ceng::PlatformWindow *window;

	Ceng::UINT32 windowOptions = 0;

	if (fullScreenMode)
	{
		windowOptions = Ceng::E_WindowOptions::popup | Ceng::E_WindowOptions::topmost;
	}
	else
	{
		windowOptions = Ceng::E_WindowOptions::center_x
			| Ceng::E_WindowOptions::center_y;

		windowOptions |= Ceng::E_WindowOptions::own_device_context;

		//windowOptions |= Ceng::E_WindowOptions::resizable;
	}

	Ceng_CreateWindow(programName, 0, 0, resX, resY,
		windowOptions, nullptr,
		&window);

	if (window == nullptr)
	{
		return 0;
	}

	Ceng::CPU_Info *cpuData;

	Ceng_GetCpuInfo(&cpuData);

	Ceng::E_CpuFeatureInfo::value testResult;

	cpuData->CheckFeature("sse_2", &testResult);

	if (testResult != Ceng::E_CpuFeatureInfo::ok)
	{
		window->Show();
		Ceng::String title = "Error";
		Ceng::String message = "CPU doesn't support SSE2";

		cresult = Ceng_MessageWindow(window, title, message);

		cpuData->Release();
		return 0;
	}

	cpuData->Release();

	Ceng::GraphicsSystem *renderCore = nullptr;

	cresult = Ceng_CreateGraphics3D(Ceng::RENDERER_TYPE::opengl_32, &renderCore,&engineLog);

	if (cresult != Ceng::CE_OK)
	{
		Ceng_MessageWindow(window, "Error", "Failed to initialize OpenGL 3.2");

		window->Release();
		return 0;
	}

	//**************************************************************************

	// Enumerate display adapters

	Ceng::GraphicsAdapter *adapter;
	Ceng::GraphicsAdapterDesc adapterDesc;

	Ceng::UINT32 adapterCount = renderCore->AdapterCount();

	Ceng::String out = "display adapters = ";
	out += adapterCount;

	Ceng::Log::Print(out);

	renderCore->EnumAdapter(0, &adapter);

	out = "Display adapter:";

	adapter->GetDesc(adapterDesc);

	Ceng::Log::Print(out);
	Ceng::Log::Print(adapterDesc.description);
	Ceng::Log::Print("\n");

	//**************************************************************************
	// Enumerate display modes for C32_ARGB

	/*
	Ceng::IMAGE_FORMAT::value format = Ceng::IMAGE_FORMAT::C32_XRGB;

	out = "Checking back buffer format : ";
	out += ImageFormatToString(format);
	out += "\n";

	Ceng::UINT32 modeCount = renderCore->DisplayModeCount(adapter, format);

	out += "modeCount = ";
	out += modeCount;
	out += "\n";
	Ceng::Log::Print(out);

	std::vector<Ceng::DisplayMode> modeList;

	Ceng::DisplayMode displayMode;

	for(k=0;k<modeCount;++k)
	{
	renderCore->EnumDisplayMode(adapter, k, format, displayMode);

	if (displayMode.refreshHz < 59)
	{
	++k;
	continue;
	}

	modeList.push_back(displayMode);

	out = "display mode ";
	out += modeList.size() - 1;
	out += " :\n";
	out += "format = ";
	out += ImageFormatToString(displayMode.format);
	out += "\n";
	out += "width = ";
	out += displayMode.width;
	out += "\n";
	out += "height = ";
	out += displayMode.height;
	out += "\n";
	out += "hz = ";
	out += displayMode.refreshHz;
	Ceng::Log::Print(out);
	Ceng::Log::Print("\n");
	}
	*/

	//**************************************************************************
	// Test framebuffer format support

	Ceng::IMAGE_FORMAT::value displayFormat = Ceng::IMAGE_FORMAT::C32_XBGR;
	Ceng::IMAGE_FORMAT::value backBufferFormat = Ceng::IMAGE_FORMAT::C32_ABGR;

	cresult = renderCore->CheckFramebufferSupport(adapter, displayFormat, backBufferFormat, true);

	out = "Testing frame buffer format with:\n";
	out += "display format = ";
	out += ImageFormatToString(displayFormat);
	out += "\nback buffer format = ";
	out += ImageFormatToString(backBufferFormat);
	out += "\nresult = ";

	if (cresult == Ceng::CE_OK)
	{
		out += "OK";
	}
	else
	{
		out += "FAIL";
	}

	out += "\n";

	Ceng::Log::Print(out);

	/*
	//**************************************************************************
	// Test dxt5 support

	cresult = renderCore->CheckBufferSupport(adapter, displayFormat,
	Ceng::BufferType::texture_2d,
	Ceng::IMAGE_FORMAT::dxt5,
	Ceng::BufferBinding::shader_resource,
	Ceng::BufferUsage::gpu_read_only,
	0,
	0);

	Ceng::Log::Print("Testing dxt5 support with C32_ARGB display format");

	out = "result = ";

	if (cresult == Ceng::CE_OK)
	{
	out += "OK";
	}
	else
	{
	out += "FAIL";
	}

	out += "\n";

	Ceng::Log::Print(out);
	*/
	//**************************************************************************
	// Test depth buffer support
	/*
	cresult = renderCore->CheckBufferSupport(adapter, displayFormat,
	Ceng::BufferType::back_buffer,
	Ceng::IMAGE_FORMAT::D32F_LOCKABLE,
	Ceng::BufferBinding::shader_resource,
	Ceng::BufferUsage::gpu_read_only,
	0,
	0);

	Ceng::Log::Print("Testing D24_S8 support with C32_ARGB display format");

	out = "result = ";

	if (cresult == Ceng::CE_OK)
	{
	out += "OK";
	}
	else
	{
	out += "FAIL";
	}

	out += "\n";

	Ceng::Log::Print(out);
	*/

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create keyboard

	Ceng::KeyboardDevice *keyboard;

	cresult = Ceng_GetKeyboard(window, &keyboard);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create mouse

	Ceng::MouseDevice *mouse;

	cresult = Ceng_CreateMouse(window, &mouse);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create render context

	Ceng::SwapChainDesc swapChainDesc;

	swapChainDesc.displayMode.width = resX;
	swapChainDesc.displayMode.height = resY;
	swapChainDesc.displayMode.format = Ceng::IMAGE_FORMAT::C32_XBGR;
	swapChainDesc.displayMode.refreshHz = 60;

	swapChainDesc.windowed = !fullScreenMode;
	swapChainDesc.bufferCount = 2;

	swapChainDesc.outputWindow = window;
	swapChainDesc.autoDepthStencil = true;
	swapChainDesc.autoDepthStencilFormat = Ceng::IMAGE_FORMAT::D24_S8;

	Ceng::SwapChain *swapChain;
	Ceng::RenderDevice *renderDevice;
	Ceng::RenderContext *renderContext;

	cresult = renderCore->GetRenderDevice(adapter, 0, &swapChainDesc, &swapChain, &renderDevice, &renderContext);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create vertex format

	std::vector<Ceng::VertexDeclData> progVertexDecl;

	Ceng::VertexDeclData declVar;

	declVar = Ceng::VertexDeclData(0, 0, Ceng::VTX_DATATYPE::FLOAT3, 52, "position");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 12, Ceng::VTX_DATATYPE::FLOAT3, 52, "normal");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 24, Ceng::VTX_DATATYPE::FLOAT3, 52, "tangent");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 36, Ceng::VTX_DATATYPE::FLOAT2, 52, "textureUV");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 44, Ceng::VTX_DATATYPE::FLOAT2, 52, "lightmapUV");
	progVertexDecl.push_back(declVar);

	progVertexDecl.push_back(Ceng::VTX_DECL_END);

	/*
	Ceng::VertexFormat *vertexFormat;

	cresult = renderDevice->CreateVertexFormat(progVertexDecl,
		(Ceng::VertexFormat**)&vertexFormat);

	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}
	*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize managers

	CEngine::EngineResult::value eresult;

	CEngine::TextureManager textureManager(renderDevice);

	textureManager.AddPath(assetPath + "textures/");

	auto shaderManager = CEngine::ShaderManager(renderDevice);

	shaderManager.AddPath(assetPath + "shaders/");

	CEngine::MaterialManager materialManager(&textureManager, &shaderManager);

	materialManager.AddPath(assetPath + "mesh/");

	Ceng::StringUtf8 materialLog;

	CEngine::MeshManager meshManager(&materialManager, renderDevice, 2, progVertexDecl);

	meshManager.AddPath(assetPath + "mesh/");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load textures

	Ceng::ShaderResourceViewDesc diffuseViewDesc;

	diffuseViewDesc.format = Ceng::IMAGE_FORMAT::C24_BGR;
	diffuseViewDesc.tex2d.baseMipLevel = 0;
	diffuseViewDesc.tex2d.maxMipLevel = -1;

	CEngine::TextureOptions defaultTexOptions;

	defaultTexOptions.usage = Ceng::BufferUsage::gpu_read_only;
	defaultTexOptions.cpuAccessFlags = 0;
	defaultTexOptions.sRGB = true;
	defaultTexOptions.mipLevels = 0;
	defaultTexOptions.options = 0;
	defaultTexOptions.bindFlags = Ceng::BufferBinding::shader_resource;

	Ceng::SamplerState *diffuseSampler,*lightmapSampler;

	Ceng::SamplerStateDesc samplerDesc;

	samplerDesc.addressU = Ceng::TextureAddressMode::wrap;
	samplerDesc.addressV = Ceng::TextureAddressMode::wrap;
	samplerDesc.addressW = Ceng::TextureAddressMode::wrap;

	samplerDesc.minFilter = Ceng::TextureMinFilter::linear_mip_linear;
	samplerDesc.magFilter = Ceng::TextureMagFilter::linear;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &diffuseSampler);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	samplerDesc.addressU = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressV = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressW = Ceng::TextureAddressMode::clamp;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &lightmapSampler);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Environment probe light pass

	Ceng::ShaderProgram *shaderProgram;

	std::shared_ptr<CEngine::ShaderProgram> lightProbeProg;

	eresult = shaderManager.CreateProgramFromFile("quad.vs", "light-probe.fs", lightProbeProg);
	//eresult = shaderManager.CreateProgramFromFile("quad.vs", "light-spot-shadow.fs", lightProbeProg);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("CreateProgram failed:");

		Ceng::Log::Print(lightProbeProg->managerLog);

		return 0;
	}

	shaderProgram = lightProbeProg->GetProgram();

	if (shaderProgram == nullptr)
	{
		Ceng::Log::Print("CreateProgram failed:");

		Ceng::Log::Print(lightProbeProg->managerLog);

		return 0;
	}

	if (!shaderProgram->Linked())
	{
		Ceng::Log::Print("Shader Program Log:\n");

		Ceng::StringUtf8 *errorLog;

		shaderProgram->GetLog(&errorLog);
		Ceng::Log::Print(*errorLog);

		delete errorLog;

		return 0;
	}

	// Uniforms

	Ceng::ShaderConstant *probe_windowWidth;
	cresult = shaderProgram->GetConstant("windowWidth", &probe_windowWidth);

	Ceng::ShaderConstant *probe_windowHeight;
	cresult = shaderProgram->GetConstant("windowHeight", &probe_windowHeight);

	Ceng::ShaderConstant *probe_xDilationDiv;
	cresult = shaderProgram->GetConstant("xDilationDiv", &probe_xDilationDiv);

	Ceng::ShaderConstant *probe_yDilationDiv;
	cresult = shaderProgram->GetConstant("yDilationDiv", &probe_yDilationDiv);

	Ceng::ShaderConstant *probe_zTermA;
	cresult = shaderProgram->GetConstant("zTermA", &probe_zTermA);

	Ceng::ShaderConstant *probe_zTermB;
	cresult = shaderProgram->GetConstant("zTermB", &probe_zTermB);

	Ceng::ShaderConstant *probe_gbufferColor;
	cresult = shaderProgram->GetConstant("gbufferColor", &probe_gbufferColor);

	Ceng::ShaderConstant *probe_gbufferNormal;
	cresult = shaderProgram->GetConstant("gbufferNormal", &probe_gbufferNormal);

	Ceng::ShaderConstant *probe_depthBuffer;
	cresult = shaderProgram->GetConstant("depthBuffer", &probe_depthBuffer);

	Ceng::ShaderConstant *probe_reflectionEnv;
	cresult = shaderProgram->GetConstant("reflectionEnv", &probe_reflectionEnv);

	Ceng::ShaderConstant *probe_diffuseEnv;
	cresult = shaderProgram->GetConstant("diffuseEnv", &probe_diffuseEnv);

	Ceng::ShaderConstant *probe_maxEnvLOD;
	cresult = shaderProgram->GetConstant("maxEnvLOD", &probe_maxEnvLOD);

	Ceng::ShaderConstant *probe_cameraReverse;
	cresult = shaderProgram->GetConstant("cameraReverseRotation", &probe_cameraReverse);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Environment (background) drawing pass

	std::shared_ptr<CEngine::ShaderProgram> envProgLink;

	eresult = shaderManager.CreateProgramFromFile("quad-env.vs", "draw-env.fs", envProgLink);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("CreateProgram failed:");

		Ceng::Log::Print(envProgLink->managerLog);

		return 0;
	}

	shaderProgram = envProgLink->GetProgram();

	if (shaderProgram == nullptr)
	{
		Ceng::Log::Print("CreateProgram failed:");

		Ceng::Log::Print(envProgLink->managerLog);

		return 0;
	}

	if (!shaderProgram->Linked())
	{
		Ceng::Log::Print("Shader Program Log:\n");

		Ceng::StringUtf8 *errorLog;

		shaderProgram->GetLog(&errorLog);
		Ceng::Log::Print(*errorLog);

		delete errorLog;

		return 0;
	}

	Ceng::ShaderConstant *ev_windowWidth;
	cresult = shaderProgram->GetConstant("windowWidth", &ev_windowWidth);

	Ceng::ShaderConstant *ev_windowHeight;
	cresult = shaderProgram->GetConstant("windowHeight", &ev_windowHeight);

	Ceng::ShaderConstant *ev_zNear;
	cresult = shaderProgram->GetConstant("zNear", &ev_zNear);

	Ceng::ShaderConstant *ev_xDilationDiv;
	cresult = shaderProgram->GetConstant("xDilationDiv", &ev_xDilationDiv);

	Ceng::ShaderConstant *ev_yDilationDiv;
	cresult = shaderProgram->GetConstant("yDilationDiv", &ev_yDilationDiv);

	Ceng::ShaderConstant *ev_envMap;
	cresult = shaderProgram->GetConstant("envMap", &ev_envMap);

	Ceng::ShaderConstant *ev_cameraReverse;
	cresult = shaderProgram->GetConstant("cameraReverseRotation", &ev_cameraReverse);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex format for full screen quad

	struct Vec2
	{
		Ceng::FLOAT32 x;
		Ceng::FLOAT32 y;
	};

	struct QuadVertex
	{
		Vec2 position;
		CEngine::TexelF2 texCoord;
	};

	progVertexDecl.clear();

	declVar = Ceng::VertexDeclData(0, 0, Ceng::VTX_DATATYPE::FLOAT2, sizeof(QuadVertex), "position");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 12, Ceng::VTX_DATATYPE::FLOAT2, sizeof(QuadVertex), "textureUV");
	progVertexDecl.push_back(declVar);

	progVertexDecl.push_back(Ceng::VTX_DECL_END);

	Ceng::VertexFormat *quadVertexFormat;

	cresult = renderDevice->CreateVertexFormat(progVertexDecl, (Ceng::VertexFormat**)&quadVertexFormat);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex buffer for drawing proxy buffer

	QuadVertex fullScreenQuad[4];

	fullScreenQuad[0].position.x = -1.0f;
	fullScreenQuad[0].position.y = -1.0f;
	fullScreenQuad[0].texCoord.u = 0.0f;
	fullScreenQuad[0].texCoord.v = 0.0f;

	fullScreenQuad[1].position.x = 1.0f;
	fullScreenQuad[1].position.y = -1.0f;
	fullScreenQuad[1].texCoord.u = 1.0f;
	fullScreenQuad[1].texCoord.v = 0.0f;

	fullScreenQuad[2].position.x = 1.0f;
	fullScreenQuad[2].position.y = 1.0f;
	fullScreenQuad[2].texCoord.u = 1.0f;
	fullScreenQuad[2].texCoord.v = 1.0f;

	fullScreenQuad[3].position.x = -1.0f;
	fullScreenQuad[3].position.y = 1.0f;
	fullScreenQuad[3].texCoord.u = 0.0f;
	fullScreenQuad[3].texCoord.v = 1.0f;

	Ceng::VertexBuffer *quadVertexBuffer;

	cresult = renderDevice->CreateVertexBuffer(sizeof(QuadVertex), 4,
		Ceng::BufferUsage::gpu_read_only, (Ceng::VertexBuffer**)&quadVertexBuffer);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	quadVertexBuffer->LoadData(fullScreenQuad);

	Ceng::UINT8 fullScreenIndices[6];

	fullScreenIndices[0] = 0;
	fullScreenIndices[1] = 1;
	fullScreenIndices[2] = 2;
	fullScreenIndices[3] = 0;
	fullScreenIndices[4] = 2;
	fullScreenIndices[5] = 3;

	Ceng::IndexBuffer *quadIndices;

	cresult = renderDevice->CreateIndexBuffer(1, 6, Ceng::BufferUsage::gpu_read_only,
		fullScreenIndices, (Ceng::IndexBuffer**)&quadIndices);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader program for a full screen quad

	std::shared_ptr<CEngine::ShaderProgram> quadProgram;

	//eresult = shaderManager.CreateProgramFromFile("quad.vs", "quad.fs", quadProgram);
	eresult = shaderManager.CreateProgramFromFile("quad.vs", "quad-tone-test.fs", quadProgram);
	if (eresult != CEngine::EngineResult::ok)
	{
		return 0;
	}

	Ceng::ShaderConstant *quadProgTex;

	cresult = quadProgram->GetProgram()->GetConstant("texture", &quadProgTex);
	/*
	if (cresult != Ceng::CE_OK)
	{
	return 0;
	}
	*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// G-buffer

	Ceng::Texture2dDesc gbufferDesc;

	gbufferDesc.width = resX;
	gbufferDesc.height = resY;

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

	Ceng::Texture2D *gbufferColor;
	Ceng::ShaderResourceView *gbufferColorView;
	Ceng::RenderTargetView *gbufferColorTarget;

	cresult = renderDevice->CreateTexture2D(gbufferDesc, nullptr, &gbufferColor);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color texture : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color texture : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color texture : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color texture : something odd");
			break;
		};
		return 0;
	}

	cresult = gbufferColor->GetRenderTargetView(targetDesc, &gbufferColorTarget);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color target : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color target : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color target : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color target : something odd");
			break;
		};
		return 0;
	}

	cresult = gbufferColor->GetShaderViewTex2D(diffuseViewDesc, &gbufferColorView);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color view : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color view : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color view : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer color view : something odd");
			break;
		};
		return 0;
	}

	// Use float16 format to store normals with high precision
	gbufferDesc.format = Ceng::IMAGE_FORMAT::CF16_ABGR;

	Ceng::Texture2D *gbufferNormal;
	Ceng::ShaderResourceView *gbufferNormalView;
	Ceng::RenderTargetView *gbufferNormalTarget;

	cresult = renderDevice->CreateTexture2D(gbufferDesc, nullptr, &gbufferNormal);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal surface : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal surface : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal surface : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal surface : something odd");
			break;
		};
		return 0;
	}

	cresult = gbufferNormal->GetRenderTargetView(targetDesc, &gbufferNormalTarget);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal target : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal target : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal target : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal target : something odd");
			break;
		};
		return 0;
	}

	cresult = gbufferNormal->GetShaderViewTex2D(diffuseViewDesc, &gbufferNormalView);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal view : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal view : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal view : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create G-buffer normal view : something odd");
			break;
		};
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Depth texture

	Ceng::Texture2dDesc depthDesc;

	depthDesc.width = resX;
	depthDesc.height = resY;

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

	Ceng::Texture2D *depthTexture;
	Ceng::RenderTargetView *depthTarget;
	Ceng::ShaderResourceView *depthView;

	cresult = renderDevice->CreateTexture2D(depthDesc, nullptr, &depthTexture);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	depthTexture->GetRenderTargetView(targetDesc, &depthTarget);
	depthTexture->GetShaderViewTex2D(diffuseViewDesc, &depthView);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Light accumulator blend mode

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

	Ceng::BlendState *lightBlendState;

	cresult = renderDevice->CreateBlendState(&lightBlendDesc, &lightBlendState);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HDR render target

	Ceng::Texture2dDesc hdrDesc;

	hdrDesc.width = resX;
	hdrDesc.height = resY;

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

	Ceng::Texture2D *hdrTargetTex;

	cresult = renderDevice->CreateTexture2D(hdrDesc, nullptr, &hdrTargetTex);
	if (cresult != Ceng::CE_OK)
	{
		switch (cresult)
		{
		case Ceng::CE_ERR_INVALID_PARAM:
			Ceng_MessageWindow(window, "Error", "Failed to create HDR accumulator : invalid param");
			break;
		case Ceng::CE_ERR_FAIL:
			Ceng_MessageWindow(window, "Error", "Failed to create HDR accumulator : API failure");
			break;
		case Ceng::CE_ERR_OUT_OF_VIDEO_MEMORY:
			Ceng_MessageWindow(window, "Error", "Failed to create HDR accumulator : Out of video memory");
			break;
		default:
			Ceng_MessageWindow(window, "Error", "Failed to create HDR accumulator : something odd");
			break;
		};
		return 0;
	}

	Ceng::ShaderResourceView *hdrView;

	cresult = hdrTargetTex->GetShaderViewTex2D(diffuseViewDesc, &hdrView);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	Ceng::RenderTargetView *hdrTarget;

	cresult = hdrTargetTex->GetRenderTargetView(targetDesc, &hdrTarget);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	Ceng::SamplerStateDesc nearestDesc;

	nearestDesc.addressU = Ceng::TextureAddressMode::clamp;
	nearestDesc.addressV = Ceng::TextureAddressMode::clamp;
	nearestDesc.addressW = Ceng::TextureAddressMode::clamp;

	nearestDesc.minFilter = Ceng::TextureMinFilter::nearest;
	nearestDesc.magFilter = Ceng::TextureMagFilter::nearest;

	nearestDesc.minLod = 0.0f;
	nearestDesc.maxLod = 0.0f;
	nearestDesc.mipLodBias = 0.0f;
	nearestDesc.maxAnisotrophy = 0;

	Ceng::SamplerState *nearestSampler;

	cresult = renderDevice->CreateSamplerState(nearestDesc, &nearestSampler);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Diffuse irradiance map descriptor

	Ceng::Texture2dDesc diffuseEnvDesc;

	diffuseEnvDesc.width = 16;
	diffuseEnvDesc.height = 16;

	diffuseEnvDesc.format = Ceng::IMAGE_FORMAT::CF16_ABGR;

	diffuseEnvDesc.mipLevels = 0;
	diffuseEnvDesc.arraySize = 1;

	diffuseEnvDesc.bindFlags = Ceng::BufferBinding::shader_resource;
	diffuseEnvDesc.sRGB = false;
	diffuseEnvDesc.usage = Ceng::BufferUsage::gpu_read_only;
	diffuseEnvDesc.cpuAccessFlags = Ceng::Buffer_CPU_Access::read | Ceng::Buffer_CPU_Access::write;

	diffuseEnvDesc.optionFlags = 0;
	diffuseEnvDesc.multisampleDesc.count = 0;
	diffuseEnvDesc.multisampleDesc.quality = 0;

	Ceng::ShaderResourceViewDesc diffuseEnvViewDesc;

	diffuseEnvViewDesc.dimensions = Ceng::BufferType::cube_map;
	diffuseEnvViewDesc.cubeMap.baseMipLevel = 0;
	diffuseEnvViewDesc.cubeMap.maxMipLevel = 1;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Enviroment cube map

	std::shared_ptr<CEngine::Texture> envMapHandle;

	defaultTexOptions.bindFlags = Ceng::BufferBinding::shader_resource;
	defaultTexOptions.cpuAccessFlags = 0;
	defaultTexOptions.firstMip = 0;
	defaultTexOptions.mipLevels = 0;
	defaultTexOptions.usage = Ceng::BufferUsage::gpu_read_only;
	defaultTexOptions.sRGB = true;
	defaultTexOptions.options = Ceng::BufferOptions::generate_mip_maps;

	//eresult = textureManager.LoadCubemap("env-calib.bmp", defaultTexOptions, envMapHandle);
	//eresult = textureManager.LoadCubemap("envtest.bmp", defaultTexOptions, envMapHandle);
	eresult = textureManager.LoadCubemap("envmap.bmp", defaultTexOptions, envMapHandle);
	//eresult = textureManager.LoadCubemap("env-bake.exr", defaultTexOptions, envMapHandle);
	//eresult = textureManager.LoadCubemap("envprobe000_.exr", defaultTexOptions, envMapHandle);
	if (eresult != CEngine::EngineResult::ok)
	{
		return 0;
	}

	/*
	cresult = renderDevice->CreateCubemap(envDesc, envDataDesc, &envMap);
	if (cresult != Ceng::CE_OK)
	{
	return 0;
	}
	*/

	Ceng::ShaderResourceView *envView;

	Ceng::ShaderResourceViewDesc envViewDesc;

	envViewDesc.cubeMap.baseMipLevel = 0;
	envViewDesc.cubeMap.maxMipLevel = 1;

	cresult = envMapHandle->AsCubemap()->GetShaderViewCubemap(envViewDesc, &envView);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	// Irradiance map

	Ceng::Cubemap *diffuseEnv;

	cresult = renderDevice->CreateCubemap(diffuseEnvDesc, nullptr, &diffuseEnv);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	Ceng::ShaderResourceView *diffuseEnvView;

	cresult = diffuseEnv->GetShaderViewCubemap(diffuseViewDesc, &diffuseEnvView);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	eresult = CEngine::CreateIrradianceMap(envMapHandle->AsCubemap(), diffuseEnv);
	if (eresult != CEngine::EngineResult::ok)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Room environment probe

	std::shared_ptr<CEngine::Texture> probeMapHandle;

	//eresult = textureManager.LoadCubemap("envmap.bmp", defaultTexOptions, probeMapHandle);
	eresult = textureManager.LoadCubemap("envprobe000_.exr", defaultTexOptions, probeMapHandle);
	if (eresult != CEngine::EngineResult::ok)
	{
		return 0;
	}

	Ceng::ShaderResourceView *probeView;

	cresult = probeMapHandle->AsCubemap()->GetShaderViewCubemap(envViewDesc, &probeView);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	Ceng::Cubemap *probeIrradiance;

	cresult = renderDevice->CreateCubemap(diffuseEnvDesc, nullptr, &probeIrradiance);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	Ceng::ShaderResourceView *probeIrradianceView;

	cresult = probeIrradiance->GetShaderViewCubemap(diffuseViewDesc, &probeIrradianceView);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	eresult = CEngine::CreateIrradianceMap(probeMapHandle->AsCubemap(), probeIrradiance);
	if (eresult != CEngine::EngineResult::ok)
	{
		return 0;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Object data

	Ceng::Matrix4 objectFullTransform;

	Ceng::Matrix4 normalTransform;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Front buffer target

	Ceng::RenderTargetView *frontBufferTarget;

	cresult = renderContext->GetFrontBufferTarget(&frontBufferTarget);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Static render settings

	renderContext->SetViewport(0, 0, 640, 480);

	Ceng::DepthStencilDesc gbufferDepthDesc;

	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::LESS;
	gbufferDepthDesc.depthWrite = true;
	gbufferDepthDesc.stencilEnable = false;

	Ceng::DepthStencilState *gbufferDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &gbufferDepthState);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	// Depth test for environment drawing
	// Pass only if depth buffer value = 1 (far plane)

	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthWrite = false;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::EQUAL;

	Ceng::DepthStencilState *envDrawDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &envDrawDepthState);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	// Depth test for light drawing
	// Pass only if depth buffer value != 1 (far plane)

	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthWrite = false;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::NOT_EQUAL;

	Ceng::DepthStencilState *lightDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &lightDepthState);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	gbufferDepthDesc.depthEnable = false;

	Ceng::DepthStencilState *postDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &postDepthState);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	Ceng::RasterizerState rasterizerState;

	rasterizerState.cullingMode = Ceng::CULLING_MODE::BACK;
	//rasterizerState.cullingMode = Ceng::CULLING_MODE::NONE;
	rasterizerState.frontClockwise = false;
	rasterizerState.scissorEnable = false;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Lighting pass shader programs

	//CEngine::PointLightShader pointLightShader(shaderManager);

	//CEngine::DirectionLightShader dirLightShader(shaderManager);

	//CEngine::SpotLightShader spotLightShader(shaderManager);
	CEngine::SpotShadowShader spotShadowShader(shaderManager);

	CEngine::DepthPass depthPass(renderDevice,shaderManager);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Entity dictionary

	std::unordered_map<Ceng::StringUtf8, std::shared_ptr<CEngine::Entity>> entityDict;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load test meshes

	Ceng::Log::Print("testing");

	std::shared_ptr<CEngine::Mesh> cubeMesh;

	eresult = meshManager.LoadMesh("cube.cme", "Cube", cubeMesh);

	std::shared_ptr<CEngine::Mesh> wallMesh;

	//eresult = meshManager.LoadMesh("wall.obj", "Wall", wallMesh);

	std::shared_ptr<CEngine::Mesh> sphereMesh;

	//eresult = meshManager.LoadMesh("sphere.obj", "Sphere", sphereMesh);

	std::shared_ptr<CEngine::Mesh> terrainMesh;

	//eresult = meshManager.LoadMesh("terrain.obj", "Plane", terrainMesh);

	std::shared_ptr<CEngine::Mesh> roomMesh;

	eresult = meshManager.LoadMesh("lightmap-test.cme", "Room", roomMesh);

	std::shared_ptr<CEngine::Entity> entity;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Room

	entity = std::make_shared<CEngine::Entity>();

	//entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-180.0f, 2.0f, 158.0f));
	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(0.0f, 0.0f, 0.0f));

	entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("mesh", std::make_shared<CEngine::MeshComponent>(roomMesh));

	entityDict["room"] = entity;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Terrain
	
	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(0.0f, 0.0f, 0.0f));

	entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("mesh", std::make_shared<CEngine::MeshComponent>(terrainMesh));

	entityDict["terrain"] = entity;
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cube

	entity = std::make_shared<CEngine::Entity>();

	//entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-200.0f, 2.0f, 150.0f));
	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(0.0f, 1.0f, 0.0f));

	auto rotComp = std::make_shared<CEngine::RotationComponent>();

	rotComp->RotateByDeltas(0, 15, 0);

	entity->AddComponent("rotation", rotComp);

	entity->AddComponent("mesh", std::make_shared<CEngine::MeshComponent>(cubeMesh));

	entityDict["cube"] = entity;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sphere

	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-200+4.0f, 2.0f, 150.0f));

	entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("mesh", std::make_shared<CEngine::MeshComponent>(sphereMesh));

	entityDict["sphere"] = entity;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Wall

	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-200.0f, 2.0f, 150-16.0f));

	//entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-200.0f, 2.0f, 150-32.0f));

	entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("mesh", std::make_shared<CEngine::MeshComponent>(wallMesh));

	//entityDict["wall"] = entity;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Test directional light

	/*
	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(0.0f, 0.0f, 8.0f));

	auto rotComp = std::make_shared<CEngine::RotationComponent>();

	rotComp->RotateByDeltas(45, -30, 0);

	entity->AddComponent("rotation", rotComp);

	entity->AddComponent("dir_light", 
		std::make_shared<CEngine::PointLightComponent>(Ceng::VectorF4(1.0f, 1.0f, 1.0f), 5.0f, 1000.0f));

	entity->AddComponent("shadowmap",
		std::make_shared<CEngine::ShadowmapComponent>());

	entityDict["sun"] = entity;
	*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sun test

	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(400.0f, 200.0f, -300.0f));

	rotComp = std::make_shared<CEngine::RotationComponent>();

	rotComp->RotateByDeltas(45, 90+45, 0);

	entity->AddComponent("rotation", rotComp);
	//entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	/*
	entity->AddComponent("spot_light",
		std::make_shared<CEngine::PointLightComponent>(Ceng::VectorF4(1.0f, 1.0f, 1.0f), 1000.0f, 10000.0f));
		*/

	entity->AddComponent("spot_light",
		std::make_shared<CEngine::PointLightComponent>(Ceng::VectorF4(1.0f, 1.0f, 1.0f), 0.0f, 10000.0f));

	entity->AddComponent("spot_data", std::make_shared<CEngine::SpotLightComponent>(90.0f, 0.0f));

	/*
	entity->AddComponent("shadowmap",
		std::make_shared<CEngine::ShadowmapComponent>(renderDevice,512,10.0f,0.001f));
		*/

	//entityDict["sun_spot"] = entity;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Test spot light

	entity = std::make_shared<CEngine::Entity>();
	
	//entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-200+4.0f, 0.0f, 150+8.0f));
	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-180.0f, 4.0f, 165.0f));

	rotComp = std::make_shared<CEngine::RotationComponent>();

	//rotComp->RotateByDeltas(0, 90, 0);
	//rotComp->RotateByDeltas(0, 15, 0);

	entity->AddComponent("rotation", rotComp);
	//entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("spot_light", 
		std::make_shared<CEngine::PointLightComponent>(Ceng::VectorF4(1.0f,1.0f,1.0f),5.0f,1000.0f));	

	entity->AddComponent("spot_data", std::make_shared<CEngine::SpotLightComponent>(90.0f, 0.0f));

	/*
	entity->AddComponent("shadowmap",
		std::make_shared<CEngine::ShadowmapComponent>(renderDevice,512,0.01,0.0001));
		*/

	//entityDict["test_light"] = entity;	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Test spot light

	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-200+4.0f, 0.0f, 150+8.0f));
	//entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(-180.0f, 4.0f, 160.0f));

	rotComp = std::make_shared<CEngine::RotationComponent>();

	//rotComp->RotateByDeltas(0, 90, 0);
	rotComp->RotateByDeltas(0, 15, 0);

	entity->AddComponent("rotation", rotComp);
	//entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("spot_light",
		std::make_shared<CEngine::PointLightComponent>(Ceng::VectorF4(1.0f, 1.0f, 1.0f), 20.0f, 1000.0f));

	entity->AddComponent("spot_data", std::make_shared<CEngine::SpotLightComponent>(45.0f, 0.0f));

	/*
	entity->AddComponent("shadowmap",
		std::make_shared<CEngine::ShadowmapComponent>(renderDevice, 512, 0.01, 0.0001));
		*/		

	//entityDict["test_light2"] = entity;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Camera data

	FPSCamera camera;

	camera.SetWorldPosition(Ceng::VectorF4(0.0f, 1.7f, 10.0f));
	//camera.SetWorldPosition(Ceng::VectorF4(-200.0f, 2.0f, 158.0f));

	//camera.RotateByDeltas(0, -90, 0);

	Ceng::Matrix4 cameraFullTransform;

	Ceng::Matrix4 projectionMatrix;

	Ceng::FLOAT32 zNear = 0.01f;
	Ceng::FLOAT32 zFar = 1000.0f;
	Ceng::FLOAT32 zDiv = 1.0f / (zFar - zNear);

	Ceng::FLOAT32 zTermA = 2.0f*zNear*zFar / (zFar - zNear);
	Ceng::FLOAT32 zTermB = (zFar + zNear) / (zFar - zNear);

	Ceng::FLOAT32 viewAngle = 70.0f;

	renderDevice->CreateProjectionMatrix(640, 480, viewAngle, zNear, zFar, &projectionMatrix);

	Ceng::FLOAT32 objectAngle = 0.0f;

	Ceng::FLOAT32 walkSpeed = 3.0f;
	Ceng::FLOAT32 runSpeed = 30.0f;

	Ceng::FLOAT32 moveSpeed = 3.0f;

	Ceng::FLOAT32 cameraRotateSpeed = 90.0f;

	Ceng::FLOAT32 xDilationDiv = 1.0f / projectionMatrix.data[0][0];
	Ceng::FLOAT32 yDilationDiv = 1.0f / projectionMatrix.data[1][1];



	renderContext->SetRasterizerState(&rasterizerState);

	///////////////////////////////////////////////////////
	// Prepare for main loop

	Ceng::Matrix4 fullVertexTransform;


	Ceng::CRESULT messageResult;
	Ceng::BOOL exitLoop = false;

	Ceng::UINT32 displayWidth, displayHeight;

	window->GetClientArea(&displayWidth, &displayHeight);

	// FPS measurement variables
	double startTime, endTime;
	int frameCount = 0;
	int fps = 0;

	startTime = Ceng_HighPrecisionTimer();

	window->Show();

	Ceng::MouseState mouseState;
	Ceng::FLOAT64 frameStart, frameEnd, frameTime;

	frameTime = 0;

	Ceng::FLOAT64 physTime, physCurrentTime;
	Ceng::FLOAT32 physDeltaTime;

	physTime = Ceng_HighPrecisionTimer();

	Ceng_ShowSystemCursor(false);
	mouse->RelativeMode(true);
	mouse->ClipToWindow(true);

	Ceng::UINT32 materialIndex = 0;

	do
	{
		physCurrentTime = Ceng_HighPrecisionTimer();

		physDeltaTime = Ceng::FLOAT32(physCurrentTime - physTime);

		if (physDeltaTime < 0.0)
		{
			physDeltaTime = 0.0;
		}

		/*
		if (physDeltaTime > 0.1)
		{
		physDeltaTime = 0.1;
		}
		*/

		physTime = Ceng_HighPrecisionTimer();

		messageResult = window->ProcessMessages();

		if (messageResult != Ceng::CE_OK)
		{
			exitLoop = true;
		}
		else
		{
			if (window->IsDestroyed())
			{
				exitLoop = true;
			}

			// Process input if program is active
			if (window->IsActive() && window->IsForeground())
			{
				Ceng_ShowSystemCursor(false);

				cresult = mouse->Restore();
				cresult = mouse->GetState(&mouseState);

				if (mouseState.xRelative != 0)
				{
					camera.RotateByDeltas(0.0f, -mouseState.xRelative*cameraRotateSpeed*physDeltaTime, 0.0f);
				}

				if (mouseState.yRelative != 0)
				{
					camera.RotateByDeltas(-mouseState.yRelative*cameraRotateSpeed*physDeltaTime, 0.0f, 0.0f);
				}

				if (window->KeyboardFocus())
				{

					cresult = keyboard->Restore();
					if (cresult != Ceng::CE_OK)
					{
						exitLoop = true;
					}

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::ESC))
					{
						exitLoop = true;
					}

					moveSpeed = walkSpeed;

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::LEFT_SHIFT) ||
						keyboard->IsPressed(Ceng::KEYBOARD_KEY::RIGHT_SHIFT))
					{
						moveSpeed = runSpeed;
					}

					// Move camera forward
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::W))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, 0.0f, -moveSpeed*physDeltaTime));
					}

					// Move camera backwards
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::S))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, 0.0f, moveSpeed*physDeltaTime));
					}

					// Move camera left
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::A))
					{
						camera.MoveByDelta(Ceng::VectorF4(-moveSpeed*physDeltaTime, 0.0f, 0.0f));
					}

					// Move camera right
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::D))
					{
						camera.MoveByDelta(Ceng::VectorF4(moveSpeed*physDeltaTime, 0.0f, 0.0f));
					}

					// Move camera up
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::Q))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, moveSpeed*physDeltaTime, 0.0f));
					}

					// Move camera down
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::E))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, -moveSpeed*physDeltaTime, 0.0f));
					}

					// Turn camera left
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::LEFT_ARROW))
					{
						camera.RotateByDeltas(0.0f, cameraRotateSpeed*physDeltaTime, 0.0f);
					}

					// Turn camera right
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::RIGHT_ARROW))
					{
						camera.RotateByDeltas(0.0f, -cameraRotateSpeed*physDeltaTime, 0.0f);
					}

					// Turn camera up
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::UP_ARROW))
					{
						camera.RotateByDeltas(cameraRotateSpeed*physDeltaTime, 0.0f, 0.0f);
					}

					// Turn camera down
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::DOWN_ARROW))
					{
						camera.RotateByDeltas(-cameraRotateSpeed*physDeltaTime, 0.0f, 0.0f);
					}

					// Rotate camera left
					// NOTE : Direction reversed because
					//        -z is used as rotation axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::HOME))
					{
						camera.RotateByDeltas(0.0f, 0.0f, cameraRotateSpeed*physDeltaTime);
					}

					// Rotate camera right
					// NOTE : Direction reversed because
					//        -z is used as rotation axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::END))
					{
						camera.RotateByDeltas(0.0f, 0.0f, -cameraRotateSpeed*physDeltaTime);
					}

					auto iter = entityDict["cube"];

					CEngine::RotationComponent *rotation;

					rotation = static_cast<CEngine::RotationComponent*>( (*iter)["rotation"].get());

					// Rotate cube counterclockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_1))
					{
						rotation->RotateByDeltas(0.0f, cameraRotateSpeed*physDeltaTime, 0.0f);
					}

					// Rotate cube clockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_2))
					{
						rotation->RotateByDeltas(0.0f, -cameraRotateSpeed*physDeltaTime, 0.0f);
					}

					// Rotate cube counterclockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_3))
					{
						rotation->RotateByDeltas(0.0f, 0.0f,cameraRotateSpeed*physDeltaTime);
					}

					// Rotate cube clockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_4))
					{
						rotation->RotateByDeltas(0.0f, 0.0f,-cameraRotateSpeed*physDeltaTime);
					}

					/*

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::F1))
					{
					renderContext->SetSamplerState(0, texSamplerNearest);
					}

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::F2))
					{
					renderContext->SetSamplerState(0, texSamplerLinear);
					}
					*/
				}

				if (window->IsResized())
				{
					window->GetClientArea(&displayWidth, &displayHeight);

					renderDevice->CreateProjectionMatrix(displayWidth,
						displayHeight,
						viewAngle, zNear, zFar,
						&projectionMatrix);

					renderContext->SetViewport(0, 0, displayWidth, displayHeight);
				}

				mouse->ClipToWindow(true);
			}
			else
			{
				Ceng_ShowSystemCursor(true);
				mouse->ClipToWindow(false);
			}

			if (window->IsVisible())
			{
				// Calculate FPS
				endTime = Ceng_HighPrecisionTimer();
				if (endTime - startTime >= 1.0)
				{
					fps = frameCount;
					frameCount = 0;

					startTime = Ceng_HighPrecisionTimer();

					captionInfo = " | ";
					captionInfo += (int)displayWidth;
					//captionInfo += mouseState.xPos;
					captionInfo += " x ";
					captionInfo += (int)displayHeight;
					//captionInfo += mouseState.yPos;
					captionInfo += " | FPS = ";
					captionInfo += fps;
					captionInfo += " | t = ";
					captionInfo += 1000.0f * (frameTime / fps);
					captionInfo += " ms";

					frameTime = 0.0f;

					window->SetCaptionExtra(captionInfo);
				}

				// Draw scene if application is visible

				frameStart = Ceng_HighPrecisionTimer();

				//////////////////////////////////////////////////////////////
				// Enable depth tests and write

				renderContext->StartScene();

				//////////////////////////////////////////////////////////////
				// Shadow map generation

				CEngine::PointLightComponent *pointLightComp;
			
				depthPass.Render(renderContext, entityDict);

				//////////////////////////////////////////////////////////////
				// Construct g-buffer

				renderContext->SetDepthStencilState(gbufferDepthState);

				renderContext->SetRasterizerState(&rasterizerState);
				renderContext->SetViewport(0, 0, displayWidth, displayHeight);

				cameraFullTransform = camera.GetFullTransformation();
				normalTransform = camera.GetRotationMatrix();

				// Disable blending

				renderContext->SetBlendState(nullptr, nullptr);

				// Render targets

				renderContext->SetRenderTarget(0, gbufferColorTarget);
				renderContext->SetRenderTarget(1, gbufferNormalTarget);
				renderContext->SetRenderTarget(2, hdrTarget);

				renderContext->SetDepth(depthTarget);

				renderContext->ClearTarget(nullptr, Ceng::CE_Color(0.0f, 0.0f, 0.0f, 0.0f));
				renderContext->ClearDepth(1.0);

				//////////////////////////////////////////////////////////////
				// Render objects

				CEngine::MeshComponent *meshComp;
				
				for (auto &iter : entityDict)
				{
					auto &entity = *iter.second;

					meshComp = static_cast<CEngine::MeshComponent*>( entity["mesh"].get());

					if (meshComp == nullptr) continue;

					meshComp->Render(renderContext, &cameraFullTransform, &normalTransform, &projectionMatrix, diffuseSampler);
				}
				
				//////////////////////////////////////////////////////////////
				// Lighting pass setup				

				// Disable depth writes
				renderContext->SetDepthStencilState(lightDepthState);

				// Render targets

				renderContext->SetRenderTarget(0, hdrTarget);
				renderContext->SetRenderTarget(1, nullptr);
				renderContext->SetRenderTarget(2, nullptr);

				//renderContext->ClearTarget(nullptr, Ceng::CE_Color(0.0f, 0.0f, 0.0f, 0.0f));

				// Full screen quad vertex data
				renderContext->SetVertexFormat(quadVertexFormat);
				renderContext->SetIndexBuffer(quadIndices);
				renderContext->SetVertexStream(0, quadVertexBuffer, sizeof(QuadVertex), 0);

				// G-buffer inputs

				renderContext->SetPixelShaderResource(0, gbufferColorView);
				renderContext->SetPixelShaderSamplerState(0, nearestSampler);

				renderContext->SetPixelShaderResource(1, gbufferNormalView);
				renderContext->SetPixelShaderSamplerState(1, nearestSampler);

				renderContext->SetPixelShaderResource(2, depthView);
				renderContext->SetPixelShaderSamplerState(2, nearestSampler);

				// Position reconstruction data

				xDilationDiv = 1.0f / projectionMatrix.data[0][0];
				yDilationDiv = 1.0f / projectionMatrix.data[1][1];

				// Additive blend mode

				renderContext->SetBlendState(lightBlendState, nullptr);
			
				//////////////////////////////////////////////////////////////
				// Point lights

				/*
				pointLightShader.Configure(renderContext,displayWidth,displayHeight,xDilationDiv,yDilationDiv,zTermA,zTermB,
					0,1,2);

				for (auto &iter : entityDict)
				{
					auto &entity = *iter.second;

					pointLightComp = static_cast<CEngine::PointLightComponent*>(entity["point_light"].get());

					if (pointLightComp == nullptr) continue;

					pointLightShader.Render(renderContext, &cameraFullTransform, pointLightComp);
				}
				*/

				//////////////////////////////////////////////////////////////
				// Spot lights

				/*
				spotLightShader.Configure(renderContext, displayWidth, displayHeight, xDilationDiv, yDilationDiv, zTermA, zTermB,
					0, 1, 2);
					*/

				spotShadowShader.Configure(renderContext, displayWidth, displayHeight, xDilationDiv, yDilationDiv, zTermA, zTermB,
					0, 1, 2);

				CEngine::SpotLightComponent *spotComp;
				CEngine::RotationComponent *rotComp;
				CEngine::PositionComponent *posComp;
				CEngine::ShadowmapComponent *shadowComp;

				for (auto &iter : entityDict)
				{
					auto &entity = *iter.second;

					pointLightComp = static_cast<CEngine::PointLightComponent*>(entity["spot_light"].get());

					if (pointLightComp == nullptr) continue;

					spotComp = static_cast<CEngine::SpotLightComponent*>(entity["spot_data"].get());

					if (spotComp == nullptr) continue;

					rotComp = static_cast<CEngine::RotationComponent*>(entity["rotation"].get());

					if (rotComp == nullptr) continue;

					posComp = static_cast<CEngine::PositionComponent*>(entity["position"].get());

					if (posComp == nullptr) continue;

					shadowComp = static_cast<CEngine::ShadowmapComponent*>(entity["shadowmap"].get());

					if (shadowComp == nullptr)
					{
						/*
						spotLightShader.Render(renderContext, &cameraFullTransform, &normalTransform,
							pointLightComp, rotComp, spotComp, posComp);
							*/
					}
					else
					{
						spotShadowShader.Render(renderContext, &cameraFullTransform, &normalTransform,
							pointLightComp, rotComp, spotComp, posComp,shadowComp,nearestSampler);
					}
				}

				//////////////////////////////////////////////////////////////
				// Directional lights

				/*
				dirLightShader.Configure(renderContext, displayWidth, displayHeight, xDilationDiv, yDilationDiv, zTermA, zTermB,
					0, 1, 2);

				for (auto &iter : entityDict)
				{
					auto &entity = *iter.second;

					pointLightComp = static_cast<CEngine::PointLightComponent*>(entity["dir_light"].get());

					if (pointLightComp == nullptr) continue;

					rotComp = static_cast<CEngine::RotationComponent*>(entity["rotation"].get());

					if (rotComp == nullptr) continue;

					dirLightShader.Render(renderContext, &normalTransform, pointLightComp,rotComp);
				}
				*/

				//////////////////////////////////////////////////////////////
				// Cube map access setup

				Ceng::Matrix4 reverseCameraRotation = camera.GetReverseRotation();

				//////////////////////////////////////////////////////////////
				// Light probe pass

				renderContext->SetShaderProgram(lightProbeProg->GetProgram());

				probe_windowWidth->SetFloat((Ceng::FLOAT32)displayWidth);
				probe_windowHeight->SetFloat((Ceng::FLOAT32)displayHeight);

				probe_xDilationDiv->SetFloat(xDilationDiv);
				probe_yDilationDiv->SetFloat(yDilationDiv);
				probe_zTermA->SetFloat(zTermA);
				probe_zTermB->SetFloat(zTermB);

				probe_gbufferColor->SetInt(0);
				probe_gbufferNormal->SetInt(1);
				probe_depthBuffer->SetInt(2);

				probe_reflectionEnv->SetInt(3);
				probe_diffuseEnv->SetInt(4);

				/////////////////////////////////////////////////////////////
				// Room environment map

				Ceng::BufferData2D data;

				probeMapHandle->AsCubemap()->GetBufferData2D(&data);

				probe_maxEnvLOD->SetFloat(Ceng::FLOAT32(data.mipLevels));				

				renderContext->SetPixelShaderResource(3, probeView);
				renderContext->SetPixelShaderSamplerState(3, diffuseSampler);

				renderContext->SetPixelShaderResource(4, probeIrradianceView);
				renderContext->SetPixelShaderSamplerState(4, diffuseSampler);

				probe_cameraReverse->SetMatrix_4x4(&reverseCameraRotation.data[0][0], true);

				renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);

				/////////////////////////////////////////////////////////////
				// Environment map lighting pass

				envMapHandle->AsCubemap()->GetBufferData2D(&data);

				probe_maxEnvLOD->SetFloat(Ceng::FLOAT32(data.mipLevels));

				renderContext->SetPixelShaderResource(3, envView);
				renderContext->SetPixelShaderSamplerState(3, diffuseSampler);

				renderContext->SetPixelShaderResource(4, diffuseEnvView);
				renderContext->SetPixelShaderSamplerState(4, diffuseSampler);

				probe_cameraReverse->SetMatrix_4x4(&reverseCameraRotation.data[0][0], true);

				//renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);

				//////////////////////////////////////////////////////////////
				// Clean up after drawing lights

				// Unmount depth texture
				renderContext->SetPixelShaderResource(2, nullptr);

				///////////////////////////////////////////////////
				// Draw environment
				
				renderContext->SetShaderProgram(envProgLink->GetProgram());

				ev_cameraReverse->SetMatrix_4x4(&reverseCameraRotation.data[0][0], true);

				ev_zNear->SetFloat(zNear);

				ev_xDilationDiv->SetFloat(xDilationDiv);
				ev_yDilationDiv->SetFloat(yDilationDiv);

				ev_windowWidth->SetFloat((Ceng::FLOAT32)displayWidth);
				ev_windowHeight->SetFloat((Ceng::FLOAT32)displayHeight);

				ev_envMap->SetInt(0);

				renderContext->SetPixelShaderResource(0, envView);
				//renderContext->SetPixelShaderResource(0, diffuseEnvView);
				//renderContext->SetPixelShaderSamplerState(0, diffuseSampler);
				//renderContext->SetPixelShaderSamplerState(0, nearestSampler);

				renderContext->SetDepthStencilState(envDrawDepthState);

				renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);

				///////////////////////////////////////////////////
				// Post process hdr backbuffer	
				
				// Disable depth tests
				renderContext->SetDepthStencilState(nullptr);
				
				// Full screen quad vertex data
				renderContext->SetVertexFormat(quadVertexFormat);
				renderContext->SetIndexBuffer(quadIndices);
				renderContext->SetVertexStream(0, quadVertexBuffer, sizeof(QuadVertex), 0);
			
				renderContext->SetShaderProgram(quadProgram->GetProgram());

				quadProgTex->SetInt(0);


				/*
				shadowComp =
					static_cast<CEngine::ShadowmapComponent*>(entityDict["test_light"]->components["shadowmap"].get());
					*/

				renderContext->SetPixelShaderResource(0, hdrView);

				//renderContext->SetPixelShaderResource(0, depthView);
				//renderContext->SetPixelShaderResource(0, shadowComp->depthView);

				renderContext->SetPixelShaderSamplerState(0, nearestSampler);

				renderContext->SetRenderTarget(0, frontBufferTarget);
	
				renderContext->SetBlendState(nullptr,nullptr);
				
				renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);
				
				renderContext->EndScene();

				swapChain->Present();

				frameEnd = Ceng_HighPrecisionTimer();
				frameTime += frameEnd - frameStart;

				frameCount++;
			}
			else
			{
			//	mouse->ClipToWindow(false);
			}
		}



	} while (exitLoop == false);

	diffuseEnvView->Release();
	diffuseEnv->Release();

	ev_xDilationDiv->Release();
	ev_yDilationDiv->Release();

	ev_cameraReverse->Release();
	ev_envMap->Release();

	ev_zNear->Release();

	ev_windowWidth->Release();
	ev_windowHeight->Release();

	envView->Release();

	quadProgTex->Release();
	quadIndices->Release();
	quadVertexBuffer->Release();
	quadVertexFormat->Release();

	nearestSampler->Release();

	postDepthState->Release();

	lightBlendState->Release();

	lightDepthState->Release();

	gbufferDepthState->Release();

	envDrawDepthState->Release();

	hdrTarget->Release();
	hdrView->Release();
	hdrTargetTex->Release();

	gbufferColorTarget->Release();
	gbufferColorView->Release();
	gbufferColor->Release();

	gbufferNormalTarget->Release();
	gbufferNormalView->Release();
	gbufferNormal->Release();

	depthTarget->Release();
	depthView->Release();
	depthTexture->Release();

	frontBufferTarget->Release();

	diffuseSampler->Release();
	lightmapSampler->Release();

	// Light probe shader uniforms

	probe_depthBuffer->Release();
	probe_gbufferColor->Release();
	probe_gbufferNormal->Release();

	probe_reflectionEnv->Release();
	probe_diffuseEnv->Release();

	probe_maxEnvLOD->Release();

	probe_windowWidth->Release();
	probe_windowHeight->Release();

	probe_xDilationDiv->Release();
	probe_yDilationDiv->Release();
	probe_zTermA->Release();
	probe_zTermB->Release();

	probe_cameraReverse->Release();

	probeView->Release();

	// Point light shader uniforms

	//vertexFormat->Release();

	swapChain->Release();
	renderContext->Release();
	renderDevice->Release();	

	keyboard->Release();
	mouse->Release();

	adapter->Release();

	renderCore->Release();

	window->Release();

	Ceng_ShowSystemCursor(true);

	Ceng::Log::Print("Program finished");

	return 0;
}

#endif

#endif