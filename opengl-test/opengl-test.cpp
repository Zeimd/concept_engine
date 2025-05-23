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


	Ceng::StringUtf8 appPath,currentPath;
	
	Ceng_ExePath(appPath);
	Ceng_CurrentPath(currentPath);

	Ceng::CRESULT cresult;

	Ceng::FileLog engineLog;
	Ceng::Log::SetLog(&engineLog);

	std::stringstream writer;

	Ceng::String logFile = appPath;
	logFile += "opengl-test-log.txt";

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

	Ceng::IMAGE_FORMAT::value displayFormat = Ceng::IMAGE_FORMAT::unorm_x8_b8_g8_r8;
	Ceng::IMAGE_FORMAT::value backBufferFormat = Ceng::IMAGE_FORMAT::unorm_a8_b8_g8_r8;

	cresult = renderCore->CheckFramebufferSupport(adapter, displayFormat, backBufferFormat, true);

	out = "Testing frame buffer format with:\n";
	out += "display format = ";
	out += CEngine::ImageFormatToString(displayFormat);
	out += "\nback buffer format = ";
	out += CEngine::ImageFormatToString(backBufferFormat);
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
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to initialize keyboard");
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create mouse

	Ceng::MouseDevice *mouse;

	cresult = Ceng_CreateMouse(window, &mouse);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to initialize keyboard");
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create render context

	Ceng::SwapChainDesc swapChainDesc;

	swapChainDesc.displayMode.width = resX;
	swapChainDesc.displayMode.height = resY;
	swapChainDesc.displayMode.format = Ceng::IMAGE_FORMAT::unorm_x8_b8_g8_r8;
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
	// Front buffer target

	Ceng::RenderTargetView* frontBufferTarget;

	cresult = renderContext->GetFrontBufferTarget(&frontBufferTarget);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Return type for concept engine functions

	CEngine::EngineResult::value eresult;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// G-buffer

	CEngine::Gbuffer* gbuffer;

	eresult = CEngine::Gbuffer::GetInstance(renderDevice, resX, resY, &gbuffer);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to create G-buffer");
		return 0;
	}
	else
	{
		Ceng::Log::Print("G-buffer created succesfully");
	}

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
		return 0;
	}

	Ceng::RasterizerState rasterizerState;

	rasterizerState.cullingMode = Ceng::CULLING_MODE::BACK;
	//rasterizerState.cullingMode = Ceng::CULLING_MODE::NONE;
	rasterizerState.frontClockwise = false;
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

	Ceng::SamplerState* nearestSampler;

	cresult = renderDevice->CreateSamplerState(nearestDesc, &nearestSampler);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("Failed to create nearest sampler");
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize managers

	CEngine::TextureManager textureManager(renderDevice);

	textureManager.AddPath(assetPath + "textures/");

	auto shaderManager = CEngine::ShaderManager(renderDevice);

	shaderManager.AddPath(assetPath + "shaders/");

	CEngine::MaterialManager materialManager(&textureManager, &shaderManager);

	materialManager.AddPath(assetPath + "mesh/");

	Ceng::StringUtf8 materialLog;

	CEngine::MeshManager meshManager(&materialManager, renderDevice, 2, progVertexDecl);

	meshManager.AddPath(assetPath + "mesh/");

	std::shared_ptr<CEngine::EnvMapManager> envMapManager;

	eresult = CEngine::EnvMapManager::GetInstance(renderDevice, &textureManager, &shaderManager, envMapManager);

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to initialize environment map manager\n");
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex format for full screen quad

	CEngine::FullScreenQuad* quad;

	eresult = CEngine::FullScreenQuad::GetInstance(renderDevice, &quad);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to create full screen quad");
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader program for tone mapping

	std::shared_ptr<CEngine::ShaderProgram> toneMapProgram;

	eresult = shaderManager.CreateProgramFromFile("quad.vs", "quad-tone-test.fs", toneMapProgram);
	if (eresult != CEngine::EngineResult::ok)
	{
		return 0;
	}

	Ceng::ShaderConstant* quadProgTex;

	cresult = toneMapProgram->GetProgram()->GetConstant("texture", &quadProgTex);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Environment (background) drawing pass

	Ceng::ShaderProgram* shaderProgram;

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
	// Lighting pass

	CEngine::LightingPass* lightingPass;

	eresult = CEngine::LightingPass::GetInstance(renderDevice, envMapManager, &lightingPass);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to initialize lighting pass");
		return 0;
	}
	else
	{
		Ceng::Log::Print("Lighting pass initialized");
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Enviroment probes

	Ceng::Log::Print("Load environment probes\n");


	CEngine::Vec3 boundaryPos = { 0.0f, 1.5f, 0.0 };
	CEngine::Vec3 boxSideHalf = { 4.0f, 2.5f, 4.0f };

	eresult = envMapManager->AddEnvMapParallaxAABB("EnvProbe_1.exr", boundaryPos, boxSideHalf);
	//eresult = envMapManager->AddEnvMap("EnvProbe_1.exr");

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to create envmap from file\n");
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////77
	// Skybox

	std::shared_ptr<CEngine::Texture> skybox, skyboxIrradiance;

	CEngine::TextureOptions skyboxOptions;

	skyboxOptions.bindFlags = Ceng::BufferBinding::shader_resource;
	skyboxOptions.usage = Ceng::BufferUsage::gpu_read_only;
	skyboxOptions.cpuAccessFlags = 0;

	skyboxOptions.sRGB = false;

	skyboxOptions.firstMip = 0;
	skyboxOptions.mipLevels = 0;

	skyboxOptions.options = Ceng::BufferOptions::generate_mip_maps;

	skyboxOptions.generateIrradianceMap = true;
	skyboxOptions.irradianceSize = 16;

	//eresult = textureManager.LoadCubemap("envmap.bmp", skyboxOptions, skybox, skyboxIrradiance);
	eresult = textureManager.LoadCubemap("EnvProbe_1.exr", skyboxOptions, skybox, skyboxIrradiance);

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to load skybox\n");
		return 0;
	}

	Ceng::ShaderResourceViewDesc envViewDesc;

	envViewDesc.cubeMap.baseMipLevel = 0;
	envViewDesc.cubeMap.maxMipLevel = 1;

	Ceng::ShaderResourceView* skyBoxView;

	cresult = skybox->AsCubemap()->GetShaderViewCubemap(envViewDesc, &skyBoxView);
	if (cresult != Ceng::CE_OK)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Object data

	Ceng::Matrix4 objectFullTransform;

	Ceng::Matrix4 normalTransform;
	

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

	//////////////////////////////////////////////////////////////////////////
	// Entity registry

	CEngine::EntityRegistry entityRegistry;

	std::shared_ptr<CEngine::Entity> entity;

	std::shared_ptr<CEngine::ComponentFactory> comp = std::make_shared<CEngine::PositionFactory>();

	eresult = entityRegistry.AddComponentFactory("position", comp);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to add component factory");
		return 0;
	}

	comp = std::make_shared<CEngine::RotationFactory>();

	eresult = entityRegistry.AddComponentFactory("rotation", comp);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to add component factory: ");
		Ceng::Log::Print("rotation");
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Room

	auto roomType = std::make_shared<CEngine::EntityType>();

	roomType->defaultComponents.emplace_back("mesh", std::make_shared<CEngine::MeshComponent>(roomMesh));

	roomType->requiredComponents.push_back("position");
	roomType->requiredComponents.push_back("rotation");

	eresult = entityRegistry.AddEntityType("room", roomType);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to add entity type");
		Ceng::Log::Print("room");
		return 0;
	}
	/*
	std::unordered_map<Ceng::StringUtf8, std::shared_ptr<CEngine::Component>> initMap;

	initMap["position"] = std::make_shared<CEngine::PositionComponent>(0.0f, 0.0f, 0.0f);
	initMap["rotation"] = std::make_shared<CEngine::RotationComponent>();
	*/

	json initJSON, rotJSON;

	initJSON["position"] = { 0.0f, 0.0f, 0.0f };

	rotJSON["rotMode"] = "EULER_XYZ";
	rotJSON["angles"] = { 0.0f, 0.0f, 0.0f };

	initJSON["rotation"] = rotJSON;

	std::shared_ptr<CEngine::Entity> roomEntity;

	eresult = entityRegistry.GetEntityInstance("room", initJSON, roomEntity);
	//eresult = entityRegistry.GetEntityInstance("room", initMap, roomEntity);

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Error: failed to get instance of:");
		Ceng::Log::Print("room");
		return 0;
	}

	entityDict["room"] = roomEntity;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Terrain
	
	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(0.0f, 0.0f, 0.0f));

	entity->AddComponent("rotation", std::make_shared<CEngine::RotationComponent>());

	entity->AddComponent("mesh", std::make_shared<CEngine::MeshComponent>(terrainMesh));

	entityDict["terrain"] = entity;
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cubes

	auto cubeType = std::make_shared<CEngine::EntityType>();

	cubeType->defaultComponents.emplace_back("mesh", std::make_shared<CEngine::MeshComponent>(cubeMesh));

	cubeType->requiredComponents.push_back("position");
	cubeType->requiredComponents.push_back("rotation");

	eresult = entityRegistry.AddEntityType("cube", cubeType);
	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Failed to add entity type");
		Ceng::Log::Print("cube");
		return 0;
	}
	std::shared_ptr<CEngine::Entity> cubeEntity;

	initJSON["position"] = { 0.0f, 2.0f, 0.0f };

	rotJSON["rotMode"] = "EULER_XYZ";
	rotJSON["angles"] = { 0.0f, 45.0f, 0.0f };

	initJSON["rotation"] = rotJSON;

	eresult = entityRegistry.GetEntityInstance("cube", initJSON, cubeEntity);

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Error: failed to get instance of:");
		Ceng::Log::Print("cube");
		return 0;
	}

	entityDict["cube"] = cubeEntity;

	initJSON["position"] = { 8.0f, 1.0f, 2.0f };

	rotJSON["rotMode"] = "EULER_XYZ";
	rotJSON["angles"] = { 0.0f, 0.0f, 0.0f };

	initJSON["rotation"] = rotJSON;

	eresult = entityRegistry.GetEntityInstance("cube", initJSON, cubeEntity);

	if (eresult != CEngine::EngineResult::ok)
	{
		Ceng::Log::Print("Error: failed to get instance of:");
		Ceng::Log::Print("cube2");
		return 0;
	}

	entityDict["cube2"] = cubeEntity;

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

	auto rotComp = std::make_shared<CEngine::RotationComponent>();

	entity = std::make_shared<CEngine::Entity>();

	entity->AddComponent("position", std::make_shared<CEngine::PositionComponent>(400.0f, 200.0f, -300.0f));

	rotComp = std::make_shared<CEngine::RotationComponent>();

	rotComp->AddRotation(CEngine::EulerMode::xyx, 45, 90+45, 0);

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
	rotComp->AddRotation(CEngine::EulerMode::xyz, 0, 15, 0);

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

	CEngine::FPSCamera camera;

	camera.SetWorldPosition(Ceng::VectorF4(0.0f, 1.7f, 10.0f));
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
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, cameraRotateSpeed*physDeltaTime, 0.0f);
					}

					// Rotate cube clockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_2))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, -cameraRotateSpeed*physDeltaTime, 0.0f);
					}

					// Rotate cube counterclockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_3))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, 0.0f,cameraRotateSpeed*physDeltaTime);
					}

					// Rotate cube clockwise around y-axis
					if (keyboard->IsPressed(Ceng::KEYBOARD_KEY::KEY_4))
					{
						rotation->AddRotation(CEngine::EulerMode::xyz,
							0.0f, 0.0f,-cameraRotateSpeed*physDeltaTime);
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
				// Shadow map generation

				CEngine::PointLightComponent *pointLightComp;
			
				depthPass.Render(renderContext, entityDict);

				//////////////////////////////////////////////////////////////
				// Construct g-buffer

				renderContext->SetDepthStencilState(gbufferDepthState);

				renderContext->SetRasterizerState(&rasterizerState);
				renderContext->SetViewport(0, 0, deferredParams.windowWidth, deferredParams.windowHeight);

				cameraFullTransform = camera.GetFullTransformation();
				normalTransform = camera.GetRotationMatrix();

				// Disable blending

				renderContext->SetBlendState(nullptr, nullptr);

				// Render targets

				renderContext->SetRenderTarget(0, gbuffer->gbufferColorTarget);
				renderContext->SetRenderTarget(1, gbuffer->gbufferNormalTarget);
				renderContext->SetRenderTarget(2, gbuffer->hdrTarget);

				renderContext->SetDepth(gbuffer->depthTarget);

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
				// Lighting pass 			

				deferredParams.xDilationDiv = 1.0f / projectionMatrix.data[0][0];
				deferredParams.yDilationDiv = 1.0f / projectionMatrix.data[1][1];

				lightingPass->Render(renderContext, gbuffer, quad, nearestSampler, &camera, &deferredParams, &envMapParams);

		
			
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

				spotShadowShader.Configure(renderContext, deferredParams.windowWidth,
					deferredParams.windowHeight, 
					deferredParams.xDilationDiv, deferredParams.yDilationDiv, 
					deferredParams.zTermA, deferredParams.zTermB,
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
				// Clean up after drawing lights

				// Unmount depth texture
				renderContext->SetPixelShaderResource(2, nullptr);

				///////////////////////////////////////////////////
				// Draw skybox
				
				renderContext->SetShaderProgram(envProgLink->GetProgram());

				ev_cameraReverse->SetMatrix_4x4(&envMapParams.cameraReverseRotation.data[0][0], true);

				ev_zNear->SetFloat(zNear);

				ev_xDilationDiv->SetFloat(deferredParams.xDilationDiv);
				ev_yDilationDiv->SetFloat(deferredParams.yDilationDiv);

				ev_windowWidth->SetFloat((Ceng::FLOAT32)deferredParams.windowWidth);
				ev_windowHeight->SetFloat((Ceng::FLOAT32)deferredParams.windowHeight);

				ev_envMap->SetInt(0);

				renderContext->SetPixelShaderResource(0, skyBoxView);
				
				
				//renderContext->SetPixelShaderResource(0, diffuseEnvView);
				renderContext->SetPixelShaderSamplerState(0, diffuseSampler);
				renderContext->SetPixelShaderSamplerState(0, nearestSampler);

				renderContext->SetDepthStencilState(envDrawDepthState);

				renderContext->DrawIndexed(Ceng::PRIMITIVE_TYPE::TRIANGLE_LIST, 0, 6);

				///////////////////////////////////////////////////
				// Post process hdr backbuffer	
				
				// Disable depth tests
				renderContext->SetDepthStencilState(nullptr);
				
				// Full screen quad vertex data
				renderContext->SetVertexFormat(quad->quadVertexFormat);
				renderContext->SetIndexBuffer(quad->quadIndices);
				renderContext->SetVertexStream(0, quad->quadVertexBuffer, sizeof(CEngine::QuadVertex), 0);
			
				renderContext->SetShaderProgram(toneMapProgram->GetProgram());

				quadProgTex->SetInt(0);


				/*
				shadowComp =
					static_cast<CEngine::ShadowmapComponent*>(entityDict["test_light"]->components["shadowmap"].get());
					*/

				renderContext->SetPixelShaderResource(0, gbuffer->hdrView);

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

	//skyboxIrradianceView->Release();

	//diffuseEnv->Release();

	ev_xDilationDiv->Release();
	ev_yDilationDiv->Release();

	ev_cameraReverse->Release();
	ev_envMap->Release();

	ev_zNear->Release();

	ev_windowWidth->Release();
	ev_windowHeight->Release();

	quadProgTex->Release();

	nearestSampler->Release();

	postDepthState->Release();

	//lightDepthState->Release();

	gbufferDepthState->Release();

	envDrawDepthState->Release();

	frontBufferTarget->Release();

	diffuseSampler->Release();
	lightmapSampler->Release();

	skyBoxView->Release();

	delete gbuffer;
	delete quad;
	delete lightingPass;

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