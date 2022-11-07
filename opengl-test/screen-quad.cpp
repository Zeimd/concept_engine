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
		return EngineResult::fail;
	}

	/*
	CEngine::QuadVertex fullScreenQuad[4];

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
	*/

	Ceng::VertexBuffer* quadVertexBuffer;

	cresult = renderDevice->CreateVertexBuffer(sizeof(CEngine::QuadVertex), 4,
		Ceng::BufferUsage::gpu_read_only, (Ceng::VertexBuffer**)&quadVertexBuffer);
	if (cresult != Ceng::CE_OK)
	{
		quadVertexFormat->Release();
		return EngineResult::fail;
	}

	quadVertexBuffer->LoadData(fullScreenQuad);

	/*
	Ceng::UINT8 fullScreenIndices[6];

	fullScreenIndices[0] = 0;
	fullScreenIndices[1] = 1;
	fullScreenIndices[2] = 2;
	fullScreenIndices[3] = 0;
	fullScreenIndices[4] = 2;
	fullScreenIndices[5] = 3;
	*/

	Ceng::IndexBuffer* quadIndices;

	cresult = renderDevice->CreateIndexBuffer(1, 6, Ceng::BufferUsage::gpu_read_only,
		fullScreenIndices, (Ceng::IndexBuffer**)&quadIndices);
	if (cresult != Ceng::CE_OK)
	{
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
		quadVertexBuffer->Release();
		quadVertexFormat->Release();
		quadIndices->Release();

		return EngineResult::out_of_memory;
	}

	*output = temp;

	return EngineResult::ok;
}
