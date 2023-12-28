/*****************************************************************************
*
* screen-quad.cpp
*
* By Jari Korkala 11/2022
*
*****************************************************************************/

#include "screen-quad.h"
#include "vertex-formats.h"

using namespace CEngine;

CEngine::QuadVertex FullScreenQuad::fullScreenQuad[4] =
{
	{-1.0f, -1.0f, 0.0f, 0.0f},

	{1.0f, -1.0f, 1.0f, 0.0f},

	{1.0f, 1.0f, 1.0f, 1.0f},

	{-1.0f, 1.0f, 0.0f, 1.0f}

};

Ceng::UINT8 FullScreenQuad::fullScreenIndices[6] = {0,1,2,0,2,3};

FullScreenQuad::FullScreenQuad()
{

}

FullScreenQuad::FullScreenQuad(Ceng::VertexFormat* quadVertexFormat, Ceng::VertexBuffer* quadVertexBuffer, Ceng::IndexBuffer* quadIndices)
	: quadVertexFormat(quadVertexFormat), quadVertexBuffer(quadVertexBuffer), quadIndices(quadIndices)
{

}

FullScreenQuad::~FullScreenQuad()
{
	quadVertexBuffer->Release();
	quadIndices->Release();

	quadVertexFormat->Release();
}

EngineResult::value FullScreenQuad::GetInstance(Ceng::RenderDevice* renderDevice, FullScreenQuad** output)
{
	*output = nullptr;

	std::vector<Ceng::VertexDeclData> progVertexDecl;

	Ceng::VertexDeclData declVar;

	declVar = Ceng::VertexDeclData(0, 0, Ceng::VTX_DATATYPE::FLOAT2, sizeof(CEngine::QuadVertex), "position");
	progVertexDecl.push_back(declVar);

	declVar = Ceng::VertexDeclData(0, 12, Ceng::VTX_DATATYPE::FLOAT2, sizeof(CEngine::QuadVertex), "textureUV");
	progVertexDecl.push_back(declVar);

	progVertexDecl.push_back(Ceng::VTX_DECL_END);

	Ceng::VertexFormat* quadVertexFormat;

	Ceng::CRESULT cresult;

	cresult = renderDevice->CreateVertexFormat(progVertexDecl, (Ceng::VertexFormat**)&quadVertexFormat);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("FullScreenQuad::GetInstance : failed to create vertex format");
		Ceng::Log::Print(cresult);
		return EngineResult::fail;
	}

	Ceng::VertexBuffer* quadVertexBuffer;

	cresult = renderDevice->CreateVertexBuffer(sizeof(CEngine::QuadVertex), 4,
		Ceng::BufferUsage::gpu_read_only, (Ceng::VertexBuffer**)&quadVertexBuffer);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("FullScreenQuad::GetInstance : failed to create vertex buffer");
		Ceng::Log::Print(cresult);
		quadVertexFormat->Release();
		return EngineResult::fail;
	}

	quadVertexBuffer->LoadData(fullScreenQuad);

	Ceng::IndexBuffer* quadIndices;

	cresult = renderDevice->CreateIndexBuffer(1, 6, Ceng::BufferUsage::gpu_read_only,
		fullScreenIndices, (Ceng::IndexBuffer**)&quadIndices);
	if (cresult != Ceng::CE_OK)
	{
		Ceng::Log::Print("FullScreenQuad::GetInstance : failed to create index buffer");
		Ceng::Log::Print(cresult);
		quadVertexBuffer->Release();
		quadVertexFormat->Release();

		return EngineResult::fail;
	}

	FullScreenQuad* temp;

	try
	{
		temp = new FullScreenQuad(quadVertexFormat, quadVertexBuffer, quadIndices);
	}
	catch (std::bad_alloc&)
	{
		Ceng::Log::Print("FullScreenQuad::GetInstance : failed to create object");
		quadVertexBuffer->Release();
		quadVertexFormat->Release();
		quadIndices->Release();

		return EngineResult::out_of_memory;
	}

	*output = temp;

	return EngineResult::ok;
}
