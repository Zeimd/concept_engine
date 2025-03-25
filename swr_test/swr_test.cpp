#include <ceng.h>

#include <ceng/lib/liblog.h>

#include <ceng/lib/timerlib.h>

#include "engine-v2.h"

#include <upng.h>

const Ceng::CRESULT EnumBackbufferFormats(Ceng::GraphicsSystem* renderCore, Ceng::GraphicsAdapter* adapter, Ceng::IMAGE_FORMAT::value format)
{
	Ceng::StringUtf8 out;

	out = "Checking back buffer format : ";
	out += CEngine::ImageFormatToString(format);
	out += "\n";

	Ceng::UINT32 modeCount = renderCore->DisplayModeCount(adapter, format);

	out += "modeCount = ";
	out += modeCount;
	out += "\n";
	Ceng::Log::Print(out);

	std::vector<Ceng::DisplayMode> modeList;

	Ceng::DisplayMode displayMode;

	for (int k = 0; k < modeCount; ++k)
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
		out += CEngine::ImageFormatToString(displayMode.format);
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

	return Ceng::CE_OK;
}


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

	double loadStart, loadEnd;

	loadStart = Ceng_HighPrecisionTimer();


	Ceng::StringUtf8 appPath, currentPath;

	Ceng_ExePath(appPath);
	Ceng_CurrentPath(currentPath);

	Ceng::CRESULT cresult;

	Ceng::FileLog engineLog;
	Ceng::Log::SetLog(&engineLog);

	std::stringstream writer;

	Ceng::String logFile = appPath;
	logFile += "swr-test-log.txt";

	cresult = engineLog.OpenFile(logFile.ToCString());
	if (cresult != Ceng::CE_OK)
	{
		Ceng::String title = "Error";
		Ceng::String message = "Failed to open log file: ";
		message += logFile;

		cresult = Ceng_MessageWindow(NULL, title, message);
		Ceng::Log::SetLog(NULL);
		return 0;
	}

	cresult = Ceng::Log::Print("Program started");
	if (cresult != Ceng::CE_OK)
	{
		Ceng::String title = "Error";
		Ceng::String message = "Failed to write to log file";

		cresult = Ceng_MessageWindow(NULL, title, message);
		Ceng::Log::SetLog(NULL);
		return 0;
	}

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

	Ceng::UINT32 resX = 800;
	Ceng::UINT32 resY = 600;

	Ceng::BOOL fullScreenMode = false;

	Ceng::String programName;
	Ceng::String captionInfo;

	programName = "Ceng-swr v ";
	programName += CEngine::majorVersion;
	programName += ".";
	programName += CEngine::minorVersion;
	programName += " Build ";
	programName += CEngine::build;

	Ceng::PlatformWindow* window;

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
		Ceng::Log::Print("Could not open window\n");
		return 0;
	}

	Ceng::CPU_Info* cpuData;

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

	Ceng::GraphicsSystem* renderCore = nullptr;

	cresult = Ceng_CreateGraphics3D(Ceng::RENDERER_TYPE::SOFTWARE, &renderCore, &engineLog);

	if (cresult != Ceng::CE_OK)
	{
		Ceng_MessageWindow(window, "Error", "Failed to initialize SWR");

		window->Release();
		return 0;
	}

	//**************************************************************************

	// Enumerate display adapters

	Ceng::GraphicsAdapter* adapter;
	Ceng::GraphicsAdapterDesc adapterDesc;

	Ceng::UINT32 adapterCount = renderCore->AdapterCount();

	Ceng::String out = "display adapters = ";
	out += adapterCount;
	Ceng::Log::Print(out);

	for (uint32_t k = 0; k < adapterCount; k++)
	{
		renderCore->EnumAdapter(k, &adapter);

		adapter->GetDesc(adapterDesc);

		out = "Display adapter: ";

		out += adapterDesc.deviceName;

		out += "\n";

		out += "description: ";

		out += adapterDesc.description;

		out += "\n";

		if (adapterDesc.stateFlags & Ceng::GraphicsAdapterState::active)
		{
			out += "active\n";
		}

		adapter->Release();

		Ceng::Log::Print(out);
		Ceng::Log::Print("\n");
	}

	renderCore->EnumAdapter(0, &adapter);

	Ceng::Log::Print("Using adapter:");
	Ceng::Log::Print(adapterDesc.description);
	Ceng::Log::Print("\n");

	//**************************************************************************
	// Current display mode

	Ceng::DisplayMode currentDisplayMode;

	cresult = (Ceng::CRESULT)renderCore->CurrentDisplayMode(adapter, currentDisplayMode);

	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to get current display mode\n");
		return 0;
	}


	out = "Current display mode:\n";
	out += "format = ";
	out += CEngine::ImageFormatToString(currentDisplayMode.format);
	out += "\n";
	out += "width = ";
	out += currentDisplayMode.width;
	out += "\n";
	out += "height = ";
	out += currentDisplayMode.height;
	out += "\n";
	out += "hz = ";
	out += currentDisplayMode.refreshHz;
	Ceng::Log::Print(out);
	Ceng::Log::Print("\n");

	//**************************************************************************
	// Find supported backbuffer formats

	const Ceng::IMAGE_FORMAT::value formats[] =
	{
		Ceng::IMAGE_FORMAT::unorm_a8_b8_g8_r8,
		Ceng::IMAGE_FORMAT::unorm_x8_b8_g8_r8,

		Ceng::IMAGE_FORMAT::unorm_a8_r8_g8_b8,
		Ceng::IMAGE_FORMAT::unorm_x8_r8_g8_b8,
	};

	int iters = sizeof(formats) / sizeof(Ceng::IMAGE_FORMAT::value);

	for (int k = 0; k < iters; k++)
	{
		EnumBackbufferFormats(renderCore, adapter, formats[k]);
	}
	

	//**************************************************************************
	// Test framebuffer format support

	/*
	Ceng::IMAGE_FORMAT::value displayFormat = Ceng::IMAGE_FORMAT::C32_XBGR;
	//Ceng::IMAGE_FORMAT::value backBufferFormat = Ceng::IMAGE_FORMAT::C32_ABGR;
	Ceng::IMAGE_FORMAT::value backBufferFormat = Ceng::IMAGE_FORMAT::C32_XBGR;

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
		out += "FAIL: ";
		out += cresult;
	}

	out += "\n";

	Ceng::Log::Print(out);
	*/

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

	Ceng::KeyboardDevice* keyboard;

	cresult = Ceng_GetKeyboard(window, &keyboard);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to initialize keyboard");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create mouse

	Ceng::MouseDevice* mouse;

	cresult = Ceng_CreateMouse(window, &mouse);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to initialize keyboard");
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create render context

	Ceng::SwapChainDesc swapChainDesc;

	memset(&swapChainDesc, 0, sizeof(Ceng::SwapChainDesc));

	swapChainDesc.displayMode.width = resX;
	swapChainDesc.displayMode.height = resY;

	// Needed for d3d9 back-end
	//swapChainDesc.displayMode.format = Ceng::IMAGE_FORMAT::C32_ARGB;
	swapChainDesc.displayMode.format = Ceng::IMAGE_FORMAT::unorm_a8_r8_g8_b8;
	swapChainDesc.displayMode.refreshHz = 60;

	swapChainDesc.windowed = !fullScreenMode;
	swapChainDesc.bufferCount = 2;

	swapChainDesc.outputWindow = window;
	swapChainDesc.autoDepthStencil = true;
	swapChainDesc.autoDepthStencilFormat = Ceng::IMAGE_FORMAT::D24_S8;

	swapChainDesc.multisampleDesc.count = 0;
	swapChainDesc.multisampleDesc.quality = 0;

	swapChainDesc.swapEffect = Ceng::SwapEffect::flip;

	swapChainDesc.presentInterval = 0;

	// NOTE: Do not remove multi_threaded. Single-threaded library is
	//       currently not among build dependencies.
	Ceng::UINT64 deviceOptions = Ceng::E_DeviceOption::multi_threaded;

	Ceng::SwapChain* swapChain;
	Ceng::RenderDevice* renderDevice;
	Ceng::RenderContext* renderContext;

	cresult = renderCore->GetRenderDevice(adapter, deviceOptions, &swapChainDesc, &swapChain, &renderDevice, &renderContext);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create render device");
		Ceng::Log::Print(cresult);

		keyboard->Release();
		mouse->Release();

		adapter->Release();
		renderCore->Release();

		window->Release();

		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Front buffer target

	Ceng::RenderTargetView* frontBufferTarget;

	cresult = swapChain->GetFrameBufferTarget(&frontBufferTarget);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to get front buffer target");
		Ceng::Log::Print(cresult);

		swapChain->Release();
		renderDevice->Release();
		renderContext->Release();

		keyboard->Release();
		mouse->Release();

		adapter->Release();
		renderCore->Release();

		window->Release();

		return 0;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Return type for concept engine functions

	CEngine::EngineResult::value eresult;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// G-buffer

	/*
	CEngine::Gbuffer* gbuffer;

	eresult = CEngine::Gbuffer::GetInstance(renderDevice, resX, resY, &gbuffer);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to create G-buffer");

		frontBufferTarget->Release();

		swapChain->Release();
		renderDevice->Release();
		renderContext->Release();

		keyboard->Release();
		mouse->Release();

		adapter->Release();
		renderCore->Release();

		window->Release();

		return 0;
	}
	else
	{
		Ceng::Log::Print("G-buffer created succesfully");
	}
	*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Static render settings

	renderContext->SetViewport(0, 0, 640, 480);

	Ceng::DepthStencilDesc gbufferDepthDesc;

	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::LESS;
	gbufferDepthDesc.depthWrite = true;
	gbufferDepthDesc.stencilEnable = false;

	Ceng::DepthStencilState* gbufferDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &gbufferDepthState);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create g-buffer depth stencil state");
		return 0;
	}

	// Depth test for environment drawing
	// Pass only if depth buffer value = 1 (far plane)

	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthWrite = false;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::EQUAL;

	Ceng::DepthStencilState* envDrawDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &envDrawDepthState);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create environment depth stencil state");
		return 0;
	}

	// Depth test for light drawing
	// Pass only if depth buffer value != 1 (far plane)

	/*
	gbufferDepthDesc.depthEnable = true;
	gbufferDepthDesc.depthWrite = false;
	gbufferDepthDesc.depthTest = Ceng::TEST_TYPE::NOT_EQUAL;

	Ceng::DepthStencilState *lightDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &lightDepthState);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}
	*/

	gbufferDepthDesc.depthEnable = false;

	Ceng::DepthStencilState* postDepthState;

	cresult = renderDevice->CreateDepthStencilState(gbufferDepthDesc, &postDepthState);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create post depth-stencil state");
		return 0;
	}

	Ceng::RasterizerState rasterizerState;

	rasterizerState.cullingMode = Ceng::CULLING_MODE::BACK;
	//rasterizerState.cullingMode = Ceng::CULLING_MODE::NONE;
	rasterizerState.frontClockwise = true;
	rasterizerState.scissorEnable = false;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Samplers

	Ceng::ShaderResourceViewDesc diffuseViewDesc;

	diffuseViewDesc.format = Ceng::IMAGE_FORMAT::unorm_b8_g8_r8;
	diffuseViewDesc.tex2d.baseMipLevel = 0;
	diffuseViewDesc.tex2d.maxMipLevel = -1;

	CEngine::TextureOptions defaultTexOptions;

	defaultTexOptions.usage = Ceng::BufferUsage::gpu_read_only;
	defaultTexOptions.cpuAccessFlags = 0;
	defaultTexOptions.sRGB = true;
	defaultTexOptions.mipLevels = 0;
	defaultTexOptions.options = 0;
	defaultTexOptions.bindFlags = Ceng::BufferBinding::shader_resource;

	Ceng::SamplerState* diffuseSampler, * lightmapSampler;

	Ceng::SamplerStateDesc samplerDesc;

	samplerDesc.addressU = Ceng::TextureAddressMode::wrap;
	samplerDesc.addressV = Ceng::TextureAddressMode::wrap;
	samplerDesc.addressW = Ceng::TextureAddressMode::wrap;

	samplerDesc.minFilter = Ceng::TextureMinFilter::linear_mip_linear;
	samplerDesc.magFilter = Ceng::TextureMagFilter::linear;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &diffuseSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create diffuse sampler");
		return 0;
	}

	samplerDesc.addressU = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressV = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressW = Ceng::TextureAddressMode::clamp;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &lightmapSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create light map sampler");
		return 0;
	}

	samplerDesc.addressU = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressV = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressW = Ceng::TextureAddressMode::clamp;

	samplerDesc.minFilter = Ceng::TextureMinFilter::nearest;
	samplerDesc.magFilter = Ceng::TextureMagFilter::nearest;

	samplerDesc.minLod = 0.0f;
	samplerDesc.maxLod = 0.0f;
	samplerDesc.mipLodBias = 0.0f;
	samplerDesc.maxAnisotrophy = 0;

	Ceng::SamplerState* nearestSampler;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &nearestSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create nearest sampler");
		return 0;
	}

	samplerDesc.addressU = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressV = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressW = Ceng::TextureAddressMode::clamp;

	samplerDesc.minFilter = Ceng::TextureMinFilter::nearest_mip_nearest;
	samplerDesc.magFilter = Ceng::TextureMagFilter::nearest;

	samplerDesc.minLod = 0.0f;
	samplerDesc.maxLod = 0.0f;
	samplerDesc.mipLodBias = 0.0f;
	samplerDesc.maxAnisotrophy = 0;

	Ceng::SamplerState* nearestMipNearestSampler;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &nearestMipNearestSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create nearest sampler");
		return 0;
	}


	samplerDesc.addressU = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressV = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressW = Ceng::TextureAddressMode::clamp;

	samplerDesc.minFilter = Ceng::TextureMinFilter::linear;
	samplerDesc.magFilter = Ceng::TextureMagFilter::linear;

	samplerDesc.minLod = 0.0f;
	samplerDesc.maxLod = 0.0f;
	samplerDesc.mipLodBias = 0.0f;
	samplerDesc.maxAnisotrophy = 0;

	Ceng::SamplerState* linearSampler;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &linearSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create nearest sampler");
		return 0;
	}

	samplerDesc.addressU = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressV = Ceng::TextureAddressMode::clamp;
	samplerDesc.addressW = Ceng::TextureAddressMode::clamp;

	samplerDesc.minFilter = Ceng::TextureMinFilter::linear_mip_linear;
	samplerDesc.magFilter = Ceng::TextureMagFilter::linear;

	samplerDesc.minLod = 0.0f;
	samplerDesc.maxLod = 0.0f;
	samplerDesc.mipLodBias = 0.0f;
	samplerDesc.maxAnisotrophy = 0;

	Ceng::SamplerState* linearMipLinearSampler;

	cresult = renderDevice->CreateSamplerState(samplerDesc, &linearMipLinearSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create nearest sampler");
		return 0;
	}

	Ceng::SamplerState* activeSampler = nearestSampler;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create vertex format

	std::vector<Ceng::VertexDeclData> progVertexDecl;

	Ceng::VertexDeclData declVar;

	const int vertexStride = 64;

	declVar = Ceng::VertexDeclData(0, 0, Ceng::VTX_DATATYPE::FLOAT4, vertexStride, Ceng::SHADER_SEMANTIC::POSITION,"position");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 16, Ceng::VTX_DATATYPE::FLOAT4, vertexStride, Ceng::SHADER_SEMANTIC::NORMAL,"normal");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 32, Ceng::VTX_DATATYPE::FLOAT4, vertexStride, Ceng::SHADER_SEMANTIC::TANGENT,"tangent");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 48, Ceng::VTX_DATATYPE::FLOAT2, vertexStride, Ceng::SHADER_SEMANTIC::TEXCOORD_0, "textureUV");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 56, Ceng::VTX_DATATYPE::FLOAT2, vertexStride, Ceng::SHADER_SEMANTIC::TEXCOORD_1, "lightmapUV");
	progVertexDecl.push_back(declVar);

	progVertexDecl.push_back(Ceng::VTX_DECL_END);

	Ceng::VertexFormat* vertexFormat;

	cresult = renderDevice->CreateVertexFormat(progVertexDecl, &vertexFormat);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create vertex format");
		Ceng::Log::Print(cresult);
		return 0;
	}

	struct Vertex
	{
		CEngine::Vec4 position;
		CEngine::Vec4 normal;
		CEngine::Vec4 tangent;
		CEngine::Vec2 textureUV;
		CEngine::Vec2 lightmapUV;
	};

	Ceng::VertexBuffer* wallVertexBuffer;

	cresult = renderDevice->CreateVertexBuffer(sizeof(Vertex), 4, Ceng::BufferUsage::gpu_read_only, &wallVertexBuffer);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create vertex buffer");
		Ceng::Log::Print(cresult);
		return 0;
	}

	std::vector<Vertex> wallData;

	Vertex buildVertex;

	Ceng::FLOAT32 halfWidth = 4.0f;
	Ceng::FLOAT32 height = 4.0f;

	buildVertex.position.x = -halfWidth;
	buildVertex.position.y = 0.0f;
	buildVertex.position.z = -10.0f;
	buildVertex.position.w =  0.0f;
	buildVertex.normal.x = 0.0f;
	buildVertex.normal.y = 0.0f;
	buildVertex.normal.z = 1.0f;
	buildVertex.normal.w = 0.0f;
	buildVertex.tangent.x = 1.0f;
	buildVertex.tangent.y = 0.0f;
	buildVertex.tangent.z = 0.0f;
	buildVertex.tangent.w = 0.0f;
	buildVertex.textureUV.x = 0.0f;
	buildVertex.textureUV.y = 0.0f;
	buildVertex.lightmapUV.x = 0.0f;
	buildVertex.lightmapUV.y = 0.0f;

	wallData.push_back(buildVertex);

	buildVertex.position.x = -halfWidth;
	buildVertex.position.y = height;
	buildVertex.position.z = -10.0f;
	buildVertex.textureUV.x = 0.0f;
	buildVertex.textureUV.y = 1.0f;

	wallData.push_back(buildVertex);

	buildVertex.position.x = halfWidth;
	buildVertex.position.y = height;
	buildVertex.position.z = -10.0f;
	buildVertex.textureUV.x = 1.0f;
	buildVertex.textureUV.y = 1.0f;

	wallData.push_back(buildVertex);

	buildVertex.position.x = halfWidth;
	buildVertex.position.y = 0.0f;
	buildVertex.position.z = -10.0f;
	buildVertex.textureUV.x = 1.0f;
	buildVertex.textureUV.y = 0.0f;

	wallData.push_back(buildVertex);

	cresult = wallVertexBuffer->LoadData(&wallData[0]);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to set vertex buffer data");
		Ceng::Log::Print(cresult);
		return 0;
	}

	Ceng::VertexShader* vShader;

	cresult = renderDevice->CreateVertexShader("", &vShader);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create vertex shader");
		Ceng::Log::Print(cresult);
		return 0;
	}

	Ceng::PixelShader* pShader;

	cresult = renderDevice->CreatePixelShader("", &pShader);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create pixel shader");
		Ceng::Log::Print(cresult);
		return 0;
	}

	Ceng::ShaderProgram* shaderProg;
	cresult = renderDevice->CreateShaderProgram(vShader, pShader, &shaderProg);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create shader program");
		Ceng::Log::Print(cresult);
		return 0;
	}

	Ceng::ShaderConstant* vs_fullVertexTransform;

	cresult = shaderProg->GetConstant("fullVertexTransform", &vs_fullVertexTransform);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to get fullVertexTransform handle");
		Ceng::Log::Print(cresult);
		return 0;
	}

	Ceng::ShaderConstant* ps_diffuseTextureUnit;

	cresult = shaderProg->GetConstant("diffuseTexture", &ps_diffuseTextureUnit);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to get diffuseTexture handle");
		Ceng::Log::Print(cresult);
		return 0;
	}

	//**********************************************************
	// Create texture

	Ceng::Texture2D* fallBackTexture = nullptr;

	Ceng::Texture2dDesc textureDesc;

	textureDesc.width = 8;
	textureDesc.height = 8;
	textureDesc.format = Ceng::IMAGE_FORMAT::unorm_a8_r8_g8_b8;
	//textureDesc.format = Ceng::IMAGE_FORMAT::C24_RGB;

	textureDesc.multisampleDesc.count = 0;
	textureDesc.multisampleDesc.quality = 0;
	textureDesc.sRGB = false;
	textureDesc.usage = Ceng::BufferUsage::gpu_read_only;
	textureDesc.bindFlags = Ceng::BufferBinding::shader_resource;
	textureDesc.cpuAccessFlags = Ceng::Buffer_CPU_Access::write;
	

	textureDesc.arraySize = 1;
	textureDesc.mipLevels = 1;// 8;
	textureDesc.optionFlags = Ceng::BufferOptions::generate_mip_maps;

	// Initial data

	struct Color32
	{
		Ceng::UINT8 blue;
		Ceng::UINT8 green;
		Ceng::UINT8 red;
		Ceng::UINT8 alpha;
	};

	Color32 textureData[8][8];

	Color32 white = { 255, 255, 255, 0 };
	Color32 orange = { 0, 69, 255, 0 };

	Ceng::UINT32 k, j;

	for (k = 0; k < 8; ++k)
	{
		for (j = 0; j < 8; ++j)
		{
			textureData[k][j] = white;
		}
	}

	textureData[0][0] = orange;
	textureData[1][1] = orange;
	textureData[2][2] = orange;
	textureData[3][3] = orange;
	textureData[4][4] = orange;
	textureData[5][5] = orange;
	textureData[6][6] = orange;
	textureData[7][7] = orange;

	textureData[0][7] = orange;
	textureData[1][6] = orange;
	textureData[2][5] = orange;
	textureData[3][4] = orange;
	textureData[4][3] = orange;
	textureData[5][2] = orange;
	textureData[6][1] = orange;
	textureData[7][0] = orange;

	Ceng::SubResourceData textureFill;

	textureFill.sourcePtr = &textureData[0][0];
	textureFill.rowPitch = 4 * 8;
	textureFill.depthPitch = 4 * 8 * 8;

	textureDesc.optionFlags = 0;

	cresult = renderDevice->CreateTexture2D(textureDesc, &textureFill, &fallBackTexture);

	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Error : Built-in fallback texture creation failed\n");
		Ceng::Log::Print(cresult);
		return 0;
	}

	Ceng::ShaderResourceView* texView;

	Ceng::ShaderResourceViewDesc texViewDesc;

	texViewDesc.dimensions = Ceng::BufferType::texture_2d;
	texViewDesc.format = textureDesc.format;
	texViewDesc.tex2d.baseMipLevel = 0;
	texViewDesc.tex2d.maxMipLevel = 8;

	cresult = renderDevice->CreateShaderResourceView(fallBackTexture, texViewDesc, &texView);

	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Error : Failed to create shader resource view\n");
		Ceng::Log::Print(cresult);
		return 0;
	}

	
	CEngine::TextureManager textureManager(renderDevice);

	textureManager.AddPath(assetPath + "textures/");

	CEngine::TextureOptions texOptions;

	texOptions.bindFlags = Ceng::BufferBinding::shader_resource;
	texOptions.cpuAccessFlags = Ceng::Buffer_CPU_Access::write;
	texOptions.firstMip = 0;
	texOptions.generateIrradianceMap = false;
	texOptions.irradianceSize = 0;
	texOptions.mipLevels = 0;
	texOptions.options = Ceng::BufferOptions::generate_mip_maps;
	texOptions.sRGB = true;
	texOptions.usage = Ceng::BufferUsage::gpu_read_only;

	std::shared_ptr<CEngine::Texture> brickWallTex;

	eresult = textureManager.LoadTexture2D("brickwall.png", texOptions, brickWallTex);

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Error : Failed to load texture from file\n");
		Ceng::Log::Print(brickWallTex->managerLog);
		Ceng::Log::Print(eresult);
		return 0;
	}

	Ceng::ShaderResourceView* brickWallView;

	texViewDesc.dimensions = Ceng::BufferType::texture_2d;
	texViewDesc.format = Ceng::IMAGE_FORMAT::unorm_a8_r8_g8_b8;
	texViewDesc.tex2d.baseMipLevel = 0;
	texViewDesc.tex2d.maxMipLevel = 11;

	cresult = brickWallTex->AsTexture2D()->GetShaderViewTex2D(texViewDesc, &brickWallView);

	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Error : Failed to create shader resource view\n");
		Ceng::Log::Print(cresult);
		return 0;
	}
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Object data

	Ceng::Matrix4 objectFullTransform;

	Ceng::Matrix4 normalTransform;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Camera data

	CEngine::FPSCamera camera;

	camera.SetWorldPosition(Ceng::VectorF4(0.0f, 1.7f, 0.0f));
	//camera.SetWorldPosition(Ceng::VectorF4(-200.0f, 2.0f, 158.0f));

	//camera.RotateByDeltas(0, -90, 0);

	Ceng::Matrix4 cameraFullTransform;

	Ceng::Matrix4 projectionMatrix;

	Ceng::FLOAT32 zNear = 0.01f;
	Ceng::FLOAT32 zFar = 1000.0f;
	Ceng::FLOAT32 zDiv = 1.0f / (zFar - zNear);

	Ceng::FLOAT32 viewAngle = 70.0f;

	renderDevice->CreateProjectionMatrix(640, 480, viewAngle, zNear, zFar, &projectionMatrix);

	Ceng::FLOAT32 objectAngle = 0.0f;

	Ceng::FLOAT32 walkSpeed = 3.0f;
	Ceng::FLOAT32 runSpeed = 30.0f;

	Ceng::FLOAT32 moveSpeed = 3.0f;

	Ceng::FLOAT32 cameraRotateSpeed = 90.0f;

	CEngine::DeferredPassCommonParams deferredParams;

	deferredParams.zTermA = 2.0f * zNear * zFar / (zFar - zNear);
	deferredParams.zTermB = (zFar + zNear) / (zFar - zNear);
	deferredParams.xDilationDiv = 1.0f / projectionMatrix.data[0][0];
	deferredParams.yDilationDiv = 1.0f / projectionMatrix.data[1][1];
	deferredParams.windowWidth = 0;
	deferredParams.windowHeight = 0;

	deferredParams.gbufferColorSlot = 0;
	deferredParams.gbufferNormalSlot = 1;
	deferredParams.depthBufferSlot = 2;

	window->GetClientArea(&deferredParams.windowWidth, &deferredParams.windowHeight);

	CEngine::EnvMapCommonParams envMapParams;

	envMapParams.envMapSlot = 3;
	envMapParams.irradianceSlot = 4;

	renderContext->SetRasterizerState(&rasterizerState);

	loadEnd = Ceng_HighPrecisionTimer();

	double loadDuration = loadEnd - loadStart;

	Ceng::String text = "Load complete. Took ";
	text += loadDuration;
	text += " seconds";

	Ceng::Log::Print(text);


	///////////////////////////////////////////////////////
	// Prepare for main loop

	Ceng::Matrix4 fullVertexTransform;


	Ceng::CRESULT messageResult;
	Ceng::BOOL exitLoop = false;

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
					camera.RotateByDeltas(0.0f, -mouseState.xRelative * cameraRotateSpeed * physDeltaTime, 0.0f);
				}

				if (mouseState.yRelative != 0)
				{
					camera.RotateByDeltas(-mouseState.yRelative * cameraRotateSpeed * physDeltaTime, 0.0f, 0.0f);
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
						camera.MoveByDelta(Ceng::VectorF4(0.0f, 0.0f, -moveSpeed * physDeltaTime));
					}

					// Move camera backwards
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::S))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, 0.0f, moveSpeed * physDeltaTime));
					}

					// Move camera left
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::A))
					{
						camera.MoveByDelta(Ceng::VectorF4(-moveSpeed * physDeltaTime, 0.0f, 0.0f));
					}

					// Move camera right
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::D))
					{
						camera.MoveByDelta(Ceng::VectorF4(moveSpeed * physDeltaTime, 0.0f, 0.0f));
					}

					// Move camera up
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::Q))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, moveSpeed * physDeltaTime, 0.0f));
					}

					// Move camera down
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::E))
					{
						camera.MoveByDelta(Ceng::VectorF4(0.0f, -moveSpeed * physDeltaTime, 0.0f));
					}

					// Turn camera left
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::LEFT_ARROW))
					{
						camera.RotateByDeltas(0.0f, cameraRotateSpeed * physDeltaTime, 0.0f);
					}

					// Turn camera right
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::RIGHT_ARROW))
					{
						camera.RotateByDeltas(0.0f, -cameraRotateSpeed * physDeltaTime, 0.0f);
					}

					// Turn camera up
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::UP_ARROW))
					{
						camera.RotateByDeltas(cameraRotateSpeed * physDeltaTime, 0.0f, 0.0f);
					}

					// Turn camera down
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::DOWN_ARROW))
					{
						camera.RotateByDeltas(-cameraRotateSpeed * physDeltaTime, 0.0f, 0.0f);
					}

					// Rotate camera left
					// NOTE : Direction reversed because
					//        -z is used as rotation axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::HOME))
					{
						camera.RotateByDeltas(0.0f, 0.0f, cameraRotateSpeed * physDeltaTime);
					}

					// Rotate camera right
					// NOTE : Direction reversed because
					//        -z is used as rotation axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::END))
					{
						camera.RotateByDeltas(0.0f, 0.0f, -cameraRotateSpeed * physDeltaTime);
					}

					/*
					auto iter = entityDict["cube"];

					CEngine::RotationComponent* rotation;

					rotation = static_cast<CEngine::RotationComponent*>((*iter)["rotation"].get());

					// Rotate cube counterclockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_1))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, cameraRotateSpeed * physDeltaTime, 0.0f);
					}

					// Rotate cube clockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_2))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, -cameraRotateSpeed * physDeltaTime, 0.0f);
					}

					// Rotate cube counterclockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_3))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, 0.0f, cameraRotateSpeed * physDeltaTime);
					}

					// Rotate cube clockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_4))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, 0.0f, -cameraRotateSpeed * physDeltaTime);
					}
					*/

					

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::F1))
					{
						activeSampler = nearestSampler;
					}

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::F2))
					{
						activeSampler = linearSampler;
					}

					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::F3))
					{
						activeSampler = nearestMipNearestSampler;
					}
									
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::F4))
					{
						activeSampler = linearMipLinearSampler;
					}
					
					
				}

				if (window->IsResized())
				{
					window->GetClientArea(&deferredParams.windowWidth, &deferredParams.windowHeight);

					renderDevice->CreateProjectionMatrix(deferredParams.windowWidth,
						deferredParams.windowHeight,
						viewAngle, zNear, zFar,
						&projectionMatrix);

					renderContext->SetViewport(0, 0, deferredParams.windowWidth, deferredParams.windowHeight);
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
					captionInfo += (int)deferredParams.windowWidth;
					//captionInfo += mouseState.xPos;
					captionInfo += " x ";
					captionInfo += (int)deferredParams.windowHeight;
					//captionInfo += mouseState.yPos;
					captionInfo += " | FPS = ";
					captionInfo += fps;
					captionInfo += " | t = ";
					captionInfo += 1000.0f * (frameTime / fps);
					captionInfo += " ms";

					frameTime = 0.0f;

					window->SetCaptionExtra(captionInfo);
				}

				// Update render parameters

				envMapParams.cameraReverseRotation = camera.GetReverseRotation();
				camera.GetPosition(&envMapParams.cameraWorldPos);

				// Draw scene if application is visible

				frameStart = Ceng_HighPrecisionTimer();

				//////////////////////////////////////////////////////////////
				// Enable depth tests and write

				renderContext->StartScene();

				//////////////////////////////////////////////////////////////
				// Forward rendering 

				renderContext->SetDepthStencilState(gbufferDepthState);

				renderContext->SetRasterizerState(&rasterizerState);
				renderContext->SetViewport(0, 0, deferredParams.windowWidth, deferredParams.windowHeight);

				cameraFullTransform = camera.GetFullTransformation();
				normalTransform = camera.GetRotationMatrix();
				fullVertexTransform = projectionMatrix * cameraFullTransform;

				renderContext->SetBlendState(nullptr, nullptr);

				renderContext->SetRenderTarget(0, frontBufferTarget);

				renderContext->ClearTarget(frontBufferTarget, Ceng::CE_Color(0.2f, 0.2f, 0.2f, 0.0f));
				renderContext->ClearDepth(1.0);

				renderContext->SetVertexStream(0, wallVertexBuffer, vertexStride, 0);

				renderContext->SetVertexFormat(vertexFormat);

				renderContext->SetShaderProgram(shaderProg);

				vs_fullVertexTransform->SetMatrix_4x4(&fullVertexTransform.data[0][0], false);

				ps_diffuseTextureUnit->SetInt(0);

				renderContext->SetPixelShaderSamplerState(0, activeSampler);

				//renderContext->SetPixelShaderResource(0, texView);
				renderContext->SetPixelShaderResource(0, brickWallView);

				renderContext->DrawPrimitive(Ceng::PRIMITIVE_TYPE::TRIANGLE_FAN, 0, 2);

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

	//skyboxIrradianceView->Release();

	//diffuseEnv->Release();

	//ev_xDilationDiv->Release();
	//ev_yDilationDiv->Release();

	//ev_cameraReverse->Release();
	//ev_envMap->Release();

	//ev_zNear->Release();

	//ev_windowWidth->Release();
	//ev_windowHeight->Release();

	//quadProgTex->Release();

	nearestSampler->Release();
	linearSampler->Release();

	nearestMipNearestSampler->Release();
	linearMipLinearSampler->Release();
	
	postDepthState->Release();

	//lightDepthState->Release();

	gbufferDepthState->Release();

	envDrawDepthState->Release();

	frontBufferTarget->Release();

	diffuseSampler->Release();
	lightmapSampler->Release();

	//skyBoxView->Release();

	//delete gbuffer;
	//delete quad;
	//delete lightingPass;

	// Point light shader uniforms

	//vertexFormat->Release();

	texView->Release();
	fallBackTexture->Release();

	brickWallView->Release();

	ps_diffuseTextureUnit->Release();
	vs_fullVertexTransform->Release();

	shaderProg->Release();
	pShader->Release();
	vShader->Release();

	wallVertexBuffer->Release();
	vertexFormat->Release();

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