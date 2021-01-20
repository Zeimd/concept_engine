/*****************************************************************************
*
* mesh-manager.cpp
*
* By Jari Korkala 7/6/2015
*
*****************************************************************************/

#include "mesh-manager.h"

using namespace CEngine;

MeshManager::MeshManager()
{
}

MeshManager::MeshManager(MaterialManager *materialManager, 
	Ceng::RenderDevice *renderDevice, const Ceng::UINT32 maxIndexBytes,
	const std::vector<Ceng::VertexDeclData> &vertexDecl) :
	materialManager(materialManager),renderDevice(renderDevice),vertexDecl(vertexDecl),maxIndexBytes(maxIndexBytes)
{
	maxIndexValue = 255;

	switch (maxIndexBytes)
	{
	case 2:
		maxIndexValue = 65535;
		break;
	case 4:
		maxIndexValue = -1;
		break;
	}

	vBufferMax = renderDevice->GetOptimalVertexElements();
	iBufferMax = renderDevice->GetOptimalIndexElements();

	if (maxIndexValue < vBufferMax)
	{
		vBufferMax = maxIndexValue;
	}

	Ceng::CRESULT cresult = renderDevice->CreateVertexFormat(vertexDecl,(Ceng::VertexFormat**)&vertexFormat);

	if (cresult != Ceng::CE_OK)
	{
		return;
	}
}

MeshManager::~MeshManager()
{
	vertexFormat->Release();
}

const EngineResult::value MeshManager::LoadMesh(const Ceng::StringUtf8 &file, 
	const Ceng::StringUtf8 &objectName, std::shared_ptr<Mesh> &out)
{
	Ceng::StringUtf8 finalFile, title,extension;

	GetFileInfo(file, finalFile, title,extension);

	Ceng::StringUtf8 materialFile = title + ".cma";

	ObjectError::value result;

	if (extension == "obj" || extension == "cme")
	{
		result = LoadMesh_OBJ(finalFile, materialFile,objectName, out);
	}


	return EngineResult::ok;
}

const Ceng::INT32 MeshManager::FindVertexVar(const Ceng::StringUtf8 &name)
{
	for (Ceng::UINT32 k = 0; k < vertexDecl.size(); ++k)
	{
		if (vertexDecl[k].name == name)
		{
			return k;
		}
	}

	return -1;
}
